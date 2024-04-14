#[compute]
#version 450

layout(local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

struct Dot
{
  float target_position_x;
  float target_position_y;

  float current_position_x;
  float current_position_y;

  float velocity_x;
  float velocity_y;

  float flock_index;
  float padding;
};

struct Flock
{
  float global_position_x;
  float global_position_y;

  float scale_x;
  float scale_y;

  float size_x;
  float size_y;

  float offset_x;
  float offset_y;

  float flip_h;
  float flip_v;
  
  float centered;
  float rotation;
};

layout(set = 0, binding = 0, std430) restrict buffer DotsBuffer {
  Dot data[];
}
dots;

layout(set = 0, binding = 1, std430) restrict buffer FlocksBuffer {
  Flock data[];
}
flocks;

layout(set = 0, binding = 2, std430) restrict buffer ParamsBuffer {
  float delta;
}
params;

layout(set = 0, binding = 3) uniform sampler2D noise_map;

float rand_from_seed(in uint seed) {
  int k;
  int s = int(seed);
  if (s == 0)
    s = 305420679;
  k = s / 127773;
  s = 16807 * (s - k * 127773) - 2836 * k;
  if (s < 0)
    s += 2147483647;
  seed = uint(s);
  return float(seed % uint(65536)) / 65535.0;
}

uint hash(uint x) {
  x = ((x >> uint(16)) ^ x) * uint(73244475);
  x = ((x >> uint(16)) ^ x) * uint(73244475);
  x = (x >> uint(16)) ^ x;
  return x;
}

vec2 rotate(vec2 v, float a)
{
	float s = sin(a);
	float c = cos(a);
	mat2 m = mat2(c, s, -s, c);
	return m * v;
}

void main() {
    float delta = params.delta;

    Dot dot = dots.data[gl_GlobalInvocationID.x];
    Flock flock = flocks.data[int(dot.flock_index)];

    vec2 target_position = vec2(dot.target_position_x, dot.target_position_y);

    target_position.x *= flock.scale_x;
    target_position.y *= flock.scale_y;

    if (flock.centered != 0.0)
      target_position -= vec2(flock.size_x * flock.scale_x / 2, flock.size_y * flock.scale_y / 2);

    target_position += vec2(flock.offset_x * flock.scale_x, flock.offset_y * flock.scale_y);

    if (flock.rotation != 0.0)
      target_position = rotate(target_position, flock.rotation);

    target_position += vec2(flock.global_position_x, flock.global_position_y);

    vec2 current_position = vec2(dot.current_position_x, dot.current_position_y);
    vec2 velocity = vec2(dot.velocity_x, dot.velocity_y);

    vec2 direction = target_position - current_position;
    float direction_length = length(direction);
    float force_amount = 250 * direction_length * direction_length;
    force_amount = min(force_amount, 500 * 50);
    direction /= direction_length;
    vec2 force = direction * force_amount;

    uint alt_seed1 = hash(gl_GlobalInvocationID.x + uint(1));

    vec2 wind = vec2(-force.y, force.x);
    wind = normalize(wind);
    wind *= sin(rand_from_seed(alt_seed1) * 200 + length(current_position) / 100) * min(length(velocity) * length(velocity) * 0.2, 200 * 15);

    velocity += force * params.delta;
    velocity += wind * params.delta;

    if (length(velocity) > 15)
        velocity *= 0.8;

    current_position += velocity * delta;

    dots.data[gl_GlobalInvocationID.x].current_position_x = current_position.x;
    dots.data[gl_GlobalInvocationID.x].current_position_y = current_position.y;

    dots.data[gl_GlobalInvocationID.x].velocity_x = velocity.x;
    dots.data[gl_GlobalInvocationID.x].velocity_y = velocity.y;
}