#include "flocking.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/multi_mesh.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/rd_shader_spirv.hpp>
#include <godot_cpp/classes/rd_shader_file.hpp>
#include <godot_cpp/classes/rd_texture_format.hpp>
#include <godot_cpp/classes/rd_sampler_state.hpp>
#include <godot_cpp/classes/rd_texture_view.hpp>
#include <godot_cpp/classes/rd_uniform.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>

using namespace godot;

static const int max_instances = 65536;
static const int max_flocks = 2048;

void FlockDot::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_current_position"), &FlockDot::set_current_position);
    ClassDB::bind_method(D_METHOD("get_current_position"), &FlockDot::get_current_position);
    ClassDB::add_property("FlockDot", PropertyInfo(Variant::VECTOR2, "current_position"), "set_current_position", "get_current_position");

    ClassDB::bind_method(D_METHOD("set_target_position"), &FlockDot::set_target_position);
    ClassDB::bind_method(D_METHOD("get_target_position"), &FlockDot::get_target_position);
    ClassDB::add_property("FlockDot", PropertyInfo(Variant::VECTOR2, "target_position"), "set_target_position", "get_target_position");

    ClassDB::bind_method(D_METHOD("set_velocity"), &FlockDot::set_velocity);
    ClassDB::bind_method(D_METHOD("get_velocity"), &FlockDot::get_velocity);
    ClassDB::add_property("FlockDot", PropertyInfo(Variant::VECTOR2, "velocity"), "set_velocity", "get_velocity");

    ClassDB::bind_method(D_METHOD("set_color"), &FlockDot::set_color);
    ClassDB::bind_method(D_METHOD("get_color"), &FlockDot::get_color);
    ClassDB::add_property("FlockDot", PropertyInfo(Variant::COLOR, "color"), "set_color", "get_color");
}

FlockDot::FlockDot() {
}

FlockDot::~FlockDot() {
}

void FlockDot::set_current_position(Vector2 _current_position) {
    current_position = _current_position;
}

Vector2 FlockDot::get_current_position() const {
    return current_position;
}

void FlockDot::set_target_position(Vector2 _target_position) {
    target_position = _target_position;
}

Vector2 FlockDot::get_target_position() const {
    return target_position;
}

void FlockDot::set_velocity(Vector2 _velocity) {
    velocity = _velocity;
}

Vector2 FlockDot::get_velocity() const {
    return velocity;
}

void FlockDot::set_color(Color _color) {
    color = _color;
}

Color FlockDot::get_color() const {
    return color;
}

void Flock::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_owner"), &Flock::set_owner);
    ClassDB::bind_method(D_METHOD("get_owner"), &Flock::get_owner);

    ClassDB::bind_method(D_METHOD("set_size"), &Flock::set_size);
    ClassDB::bind_method(D_METHOD("get_size"), &Flock::get_size);

    ClassDB::bind_method(D_METHOD("add_dot"), &Flock::add_dot);
}

Flock::Flock() {
}

Flock::~Flock() {
}

void Flock::set_owner(Sprite2D* _owner) {
    owner = _owner;
}

Sprite2D* Flock::get_owner() const {
    return owner;
}

void Flock::set_size(Vector2 _size) {
    size = _size;
}

Vector2 Flock::get_size() const {
    return size;
}

int Flock::get_dots_count() {
    return dots.size();
}

void Flock::add_dot(FlockDot* _dot) {
    dots.push_back(_dot);
}

void FlockingSystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_noise_texture"), &FlockingSystem::get_noise_texture);
    ClassDB::bind_method(D_METHOD("set_noise_texture", "p_noise_texture"), &FlockingSystem::set_noise_texture);
    ClassDB::add_property("FlockingSystem", PropertyInfo(Variant::OBJECT, "noise_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_noise_texture", "get_noise_texture");

    ClassDB::bind_method(D_METHOD("add_flock"), &FlockingSystem::add_flock);
    ClassDB::bind_method(D_METHOD("remove_flock"), &FlockingSystem::remove_flock);
}

FlockingSystem::FlockingSystem() {
}

FlockingSystem::~FlockingSystem() {
}

// TODO: Move to functions
static FlockingSystem* process_instance = nullptr;

void FlockingSystem::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Ref<SurfaceTool> st;
    st.instantiate();
    st->begin(Mesh::PRIMITIVE_TRIANGLES);

    st->set_uv(Vector2{ 0.0f, 0.0f });
    st->add_vertex(Vector3{ -0.5f, -0.5f, 0.0f });

    st->set_uv(Vector2{ 0.0f, 1.0f });
    st->add_vertex(Vector3{ -0.5f, 0.5f, 0.0f });

    st->set_uv(Vector2{ 1.0f, 1.0f });
    st->add_vertex(Vector3{ 0.5f, 0.5f, 0.0f });

    st->set_uv(Vector2{ 1.0f, 0.0f });
    st->add_vertex(Vector3{ 0.5f, -0.5f, 0.0f });

    st->set_uv(Vector2{ 0.0f, 0.0f });
    st->add_vertex(Vector3{ -0.5f, -0.5f, 0.0f });

    st->set_uv(Vector2{ 1.0f, 1.0f });
    st->add_vertex(Vector3{ 0.5f, 0.5f, 0.0f });

    st->index();

    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    st->commit(mesh);

    Ref<MultiMesh> multimesh;
    multimesh.instantiate();

    multimesh->set_mesh(mesh);
    multimesh->set_use_colors(true);
    multimesh->set_transform_format(MultiMesh::TransformFormat::TRANSFORM_2D);
    multimesh->set_instance_count(max_instances);
    multimesh->set_visible_instance_count(max_instances);

    set_multimesh(multimesh);

    rd = RenderingServer::get_singleton()->create_local_rendering_device();

    Ref<RDShaderFile> shader_file = ResourceLoader::get_singleton()->load("res://flocking_package/shaders/flock_compute.glsl"); // TODO: Make configurable from the editor
    Ref<RDShaderSPIRV> shader_spirv = shader_file->get_spirv();
    RID shader = rd->shader_create_from_spirv(shader_spirv);

    // TODO: Resize in one line
    for (int i = 0; i < max_instances; i++) {
        dots_array.push_back(0.0f);
        dots_array.push_back(0.0f);

        dots_array.push_back(0.0f);
        dots_array.push_back(0.0f);

        dots_array.push_back(0.0f);
        dots_array.push_back(0.0f);

        dots_array.push_back(0.0f);
        dots_array.push_back(0.0f);
    }

    // TODO: Resize in one line
    for (int i = 0; i < max_flocks; i++) {
        flocks_array.push_back(0.0f);
        flocks_array.push_back(0.0f);

        flocks_array.push_back(0.0f);
        flocks_array.push_back(0.0f);

        flocks_array.push_back(0.0f);
        flocks_array.push_back(0.0f);

        flocks_array.push_back(0.0f);
        flocks_array.push_back(0.0f);

        flocks_array.push_back(0.0f);
        flocks_array.push_back(0.0f);

        flocks_array.push_back(0.0f);
        flocks_array.push_back(0.0f);
    }

    dots_bytes = dots_array.to_byte_array();
    flocks_bytes = flocks_array.to_byte_array();

    PackedFloat32Array params_array;
    params_array.push_back(0.0f);
    params_bytes = params_array.to_byte_array();

    dots_buffer = rd->storage_buffer_create(dots_bytes.size(), dots_bytes);
    flocks_buffer = rd->storage_buffer_create(dots_bytes.size(), dots_bytes);
    params_buffer = rd->storage_buffer_create(dots_bytes.size(), dots_bytes);

    Ref<RDSamplerState> sampler_state;
    sampler_state.instantiate();
    RID sampler = rd->sampler_create(sampler_state);

    Ref<RDTextureFormat> noise_format;
    noise_format.instantiate();

    noise_format->set_format(RenderingDevice::DataFormat::DATA_FORMAT_R8_SNORM);
    noise_format->set_width(512);
    noise_format->set_height(512);
    noise_format->set_usage_bits(RenderingDevice::TextureUsageBits::TEXTURE_USAGE_CAN_COPY_FROM_BIT | RenderingDevice::TextureUsageBits::TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice::TextureUsageBits::TEXTURE_USAGE_CAN_UPDATE_BIT);

    Ref<Image> image;
    if (!noise_texture.is_valid()) {
        image.instantiate();
    }
    image->convert(Image::Format::FORMAT_R8);

    Ref<RDTextureView> texture_view;
    texture_view.instantiate();

    noise_rid = rd->texture_create(noise_format, texture_view);
    rd->texture_update(noise_rid, 0, image->get_data());

    Ref<RDUniform> dots_uniform;
    dots_uniform.instantiate();
    dots_uniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_STORAGE_BUFFER);
    dots_uniform->set_binding(0);
    dots_uniform->add_id(dots_buffer);

    Ref<RDUniform> flocks_uniform;
    flocks_uniform.instantiate();
    flocks_uniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_STORAGE_BUFFER);
    flocks_uniform->set_binding(1);
    flocks_uniform->add_id(flocks_buffer);

    Ref<RDUniform> params_uniform;
    params_uniform.instantiate();
    params_uniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_STORAGE_BUFFER);
    params_uniform->set_binding(2);
    params_uniform->add_id(params_buffer);

    Ref<RDUniform> noise_uniform;
    noise_uniform.instantiate();
    noise_uniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_SAMPLER_WITH_TEXTURE);
    noise_uniform->set_binding(3);
    noise_uniform->add_id(sampler);
    noise_uniform->add_id(noise_rid);

    TypedArray<RDUniform> uniforms;
    uniforms.push_back(dots_uniform);
    uniforms.push_back(flocks_uniform);
    uniforms.push_back(params_uniform);
    uniforms.push_back(noise_uniform);

    uniform_set = rd->uniform_set_create(uniforms, shader, 0);
    pipeline = rd->compute_pipeline_create(shader);
}

void FlockingSystem::_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    if (is_submitted) {
        sync();
        retrieve_output();
        process_flocks();

        is_submitted = false;
    }

    update_flock_buffers();
    update_flock_array();
    update_buffers(delta);

    compute();
    is_submitted = true;

    update_mesh();
}

void FlockingSystem::set_noise_texture(Ref<Texture2D> p_noise_texture) {
    noise_texture = p_noise_texture;
}

Ref<Texture2D> FlockingSystem::get_noise_texture() const {
    return noise_texture;
}

void FlockingSystem::add_flock(Flock* flock) {
    flocks_to_add.push_back(flock);
}

void FlockingSystem::remove_flock(Ref<Flock> flock) {
    flocks_to_remove.push_back(flock);
}

void FlockingSystem::process_flocks() {
    for (int i = 0; i < flocks_to_add.size(); i++) {
        flocks.push_back(flocks_to_add[i]);
    }
    flocks_to_add.clear();

    for (int i = 0; i < flocks_to_remove.size(); i++) {
        flocks.erase(flocks_to_remove[i]);
    }
    flocks_to_remove.clear();
}

static void process_flocks_buffer(uint32_t flock_index) {
    Ref<Flock> flock = process_instance->flocks[flock_index];

    for (int dot_index = 0; dot_index < flock->dots.size(); dot_index++) {
        Ref<FlockDot> dot = flock->dots[dot_index];
        uint32_t index = flock->base_index + dot_index;

        process_instance->dots_array[index * 8] = dot->target_position.x;
        process_instance->dots_array[index * 8 + 1] = dot->target_position.y;

        process_instance->dots_array[index * 8 + 2] = dot->current_position.x;
        process_instance->dots_array[index * 8 + 3] = dot->current_position.y;

        process_instance->dots_array[index * 8 + 4] = dot->velocity.x;
        process_instance->dots_array[index * 8 + 5] = dot->velocity.y;

        process_instance->dots_array[index * 8 + 6] = flock_index;
        process_instance->dots_array[index * 8 + 7] = 0;
    }
}

void FlockingSystem::update_flock_buffers() {
    process_instance = this;

    int base_index = 0;
    for (int flock_index = 0; flock_index < flocks.size(); flock_index++) {
        Ref<Flock> flock = flocks[flock_index];

        flock->base_index = base_index;
        base_index += flock->dots.size();
    }

    WorkerThreadPool::GroupID task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp_static(process_flocks_buffer), flocks.size(), 4, true);
    WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_id);
}

void FlockingSystem::update_flock_array() {
    int index = 0;
    for (int flock_index = 0; flock_index < flocks.size(); flock_index++) {
        Ref<Flock> flock = flocks[flock_index];

        int base_index = index * 12;
        flocks_array[base_index] = flock->owner->get_global_position().x;
        flocks_array[base_index + 1] = flock->owner->get_global_position().y;

        flocks_array[base_index + 2] = flock->owner->get_global_scale().x;
        flocks_array[base_index + 3] = flock->owner->get_global_scale().y;

        flocks_array[base_index + 4] = flock->size.x;
        flocks_array[base_index + 5] = flock->size.y;

        flocks_array[base_index + 6] = flock->owner->get_offset().x;
        flocks_array[base_index + 7] = flock->owner->get_offset().y;

        flocks_array[base_index + 8] = flock->owner->is_flipped_h() ? 1.0f : 0.0f;
        flocks_array[base_index + 9] = flock->owner->is_flipped_v() ? 1.0f : 0.0f;

        flocks_array[base_index + 10] = flock->owner->is_centered() ? 1.0f : 0.0f;
        flocks_array[base_index + 11] = flock->owner->get_global_rotation();

        index++;
    }
}

void FlockingSystem::update_buffers(double delta) {
    dots_bytes = dots_array.to_byte_array();
    flocks_bytes = flocks_array.to_byte_array();

    PackedFloat32Array params_array;
    params_array.push_back(delta);
    params_bytes = params_array.to_byte_array();

    rd->buffer_update(dots_buffer, 0, dots_bytes.size(), dots_bytes);
    rd->buffer_update(flocks_buffer, 0, flocks_bytes.size(), flocks_bytes);
    rd->buffer_update(params_buffer, 0, params_bytes.size(), params_bytes);
}

void FlockingSystem::compute() {
    int64_t compute_list = rd->compute_list_begin();
    rd->compute_list_bind_compute_pipeline(compute_list, pipeline);
    rd->compute_list_bind_uniform_set(compute_list, uniform_set, 0);
    rd->compute_list_dispatch(compute_list, max_instances / 1024, 1, 1);
    rd->compute_list_end();

    rd->submit();
}

void FlockingSystem::sync() {
    rd->sync();
}

static void process_retrieve_flock(uint32_t flock_index) {
    Ref<Flock> flock = process_instance->flocks[flock_index];

    for (int dot_index = 0; dot_index < flock->dots.size(); dot_index++) {
        Ref<FlockDot> dot = flock->dots[dot_index];
        int index = flock->base_index + dot_index;

        dot->current_position.x = process_instance->dots_array[index * 8 + 2];
        dot->current_position.y = process_instance->dots_array[index * 8 + 3];

        dot->velocity.x = process_instance->dots_array[index * 8 + 4];
        dot->velocity.y = process_instance->dots_array[index * 8 + 5];
    }
}

void FlockingSystem::retrieve_output() {
    dots_output_buffer = rd->buffer_get_data(dots_buffer);
    dots_array = dots_output_buffer.to_float32_array();

    WorkerThreadPool::GroupID task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp_static(process_retrieve_flock), flocks.size(), 4, true);
    WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_id);
}

static void process_mesh_instance(uint32_t flock_index) {
    Ref<MultiMesh> multimesh = process_instance->get_multimesh();
    RID multimesh_rid = multimesh->get_rid();

    Ref<Flock> flock = process_instance->flocks[flock_index];

    for (int dot_index = 0; dot_index < flock->dots.size(); dot_index++) {
        Ref<FlockDot> dot = flock->dots[dot_index];
        int instance_index = flock->base_index + dot_index;

        RenderingServer::get_singleton()->multimesh_instance_set_transform_2d(multimesh_rid, instance_index, Transform2D{ 0, Vector2{10.0f, 10.0f}, 0, dot->current_position });
        RenderingServer::get_singleton()->multimesh_instance_set_color(multimesh_rid, instance_index, dot->color);
    }
}

void FlockingSystem::update_mesh() {
    WorkerThreadPool::GroupID task_id = WorkerThreadPool::get_singleton()->add_group_task(callable_mp_static(process_mesh_instance), flocks.size(), 4, true);
    WorkerThreadPool::get_singleton()->wait_for_group_task_completion(task_id);

    int dots_count = 0;
    for (int flock_index = 0; flock_index < flocks.size(); flock_index++) {
        Ref<Flock> flock = flocks[flock_index];
        dots_count += flock->dots.size();
    }

    Ref<MultiMesh> multimesh = process_instance->get_multimesh();
    multimesh->set_visible_instance_count(dots_count);
}
