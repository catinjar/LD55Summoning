#ifndef FLOCKING_H
#define FLOCKING_H

#include <godot_cpp/classes/multi_mesh_instance2d.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

    class FlockDot : public RefCounted {
        GDCLASS(FlockDot, RefCounted)

    public:
        FlockDot();
        ~FlockDot();

        void set_current_position(Vector2 _current_position);
        Vector2 get_current_position() const;

        void set_target_position(Vector2 _target_position);
        Vector2 get_target_position() const;

        void set_velocity(Vector2 _velocity);
        Vector2 get_velocity() const;

        void set_color(Color _color);
        Color get_color() const;

        Vector2 current_position;
        Vector2 target_position;
        Vector2 velocity;

        Color color;

    protected:
        static void _bind_methods();
    };

    class Flock : public RefCounted {
        GDCLASS(Flock, RefCounted)

    public:
        Flock();
        ~Flock();

        TypedArray<FlockDot> dots;

        void set_owner(Sprite2D* _owner);
        Sprite2D* get_owner() const;

        void set_size(Vector2 _size);
        Vector2 get_size() const;

        int get_dots_count();
        void add_dot(FlockDot* _dot);

        Sprite2D* owner;
        Vector2 size;
        int base_index = 0;

    protected:
        static void _bind_methods();
    };

    class FlockingSystem : public MultiMeshInstance2D {
        GDCLASS(FlockingSystem, MultiMeshInstance2D)

    public:
        FlockingSystem();
        ~FlockingSystem();

        void _ready() override;
        void _process(double delta) override;

        void set_noise_texture(Ref<Texture2D> p_noise_texture);
        Ref<Texture2D> get_noise_texture() const;

        void add_flock(Flock* flock);
        void remove_flock(Ref<Flock> flock);

        void process_flocks();

        void update_flock_buffers();
        void update_flock_array();
        void update_buffers(double delta);

        void compute();
        void sync();

        void retrieve_output();
        void update_mesh();

        TypedArray<Flock> flocks;
        TypedArray<Flock> flocks_to_add;
        TypedArray<Flock> flocks_to_remove;

        RenderingDevice* rd;

        PackedFloat32Array dots_array;
        PackedFloat32Array flocks_array;

        PackedByteArray dots_bytes;
        PackedByteArray flocks_bytes;
        PackedByteArray params_bytes;

        PackedByteArray dots_output_buffer;

        RID dots_buffer;
        RID flocks_buffer;
        RID params_buffer;

        RID uniform_set;
        RID pipeline;

        RID noise_rid;
        Ref<Texture2D> noise_texture;

        bool is_submitted = false;

    protected:
        static void _bind_methods();
    };
}

#endif