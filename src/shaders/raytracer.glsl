#version 430

struct Camera {
    vec3 cam_pos;
    vec3 cam_forward;
    float cam_fov;
};

struct Ray {
    vec3 origin;
    vec3 dir;
};

layout (local_size_x = 8, local_size_y = 8) in;

layout (rgba32f, binding = 0) uniform writeonly image2D out_image;

uniform int frame;
uniform usampler3D voxel_texture;
uniform vec3 camera_position;
uniform vec3 camera_forward;

const int MAX_RAY_STEPS = 256;
const float CAMERA_FOV = radians(60.0);

Ray generate_ray(vec2 uv) {
    float h = tan(CAMERA_FOV / 2.0);
    float aspect = float(imageSize(out_image).x) / float(imageSize(out_image).y);
    float w = h * aspect;

    vec2 screen = uv * 2.0 - 1.0;

    vec3 forward = normalize(camera_forward);
    vec3 up = vec3(0,1,0);
    vec3 right = normalize(cross(forward, up));
    up = normalize(cross(right, forward));

    vec3 ray_dir = normalize(forward + screen.x * w * right + screen.y * h * up);

    return Ray(camera_position, ray_dir);
}

bool intersect_volume(Ray ray, vec3 volume_min, vec3 volume_max, out float entry, out float exit, out vec3 entry_normal) {
    vec3 safe_dir = vec3(
        abs(ray.dir.x) < 0.000001 ? 0.000001 : ray.dir.x,
        abs(ray.dir.y) < 0.000001 ? 0.000001 : ray.dir.y,
        abs(ray.dir.z) < 0.000001 ? 0.000001 : ray.dir.z
    );
    vec3 inverse_dir = 1.0 / safe_dir;
    vec3 near_plane = (volume_min - ray.origin) * inverse_dir;
    vec3 far_plane = (volume_max - ray.origin) * inverse_dir;
    vec3 near_dist = min(near_plane, far_plane);
    vec3 far_dist = max(near_plane, far_plane);

    entry = max(max(near_dist.x, near_dist.y), max(near_dist.z, 0.0));
    exit = min(far_dist.x, min(far_dist.y, far_dist.z));
    if (entry > exit) {
        return false;
    }

    entry_normal = vec3(0.0);
    if (entry == near_dist.x) {
        entry_normal = vec3(-sign(ray.dir.x), 0.0, 0.0);
    } else if (entry == near_dist.y) {
        entry_normal = vec3(0.0, -sign(ray.dir.y), 0.0);
    } else if (entry == near_dist.z) {
        entry_normal = vec3(0.0, 0.0, -sign(ray.dir.z));
    }
    return true;
}

bool trace_voxels(Ray ray, out vec3 hit_normal, out float hit_distance) {
    ivec3 volume_size = textureSize(voxel_texture, 0);
    vec3 volume_min = vec3(0.0);
    vec3 volume_max = vec3(volume_size);
    float entry;
    float exit;
    vec3 entry_normal;

    if (!intersect_volume(ray, volume_min, volume_max, entry, exit, entry_normal)) {
        return false;
    }

    vec3 position = ray.origin + ray.dir * entry;
    position += ray.dir * 0.0001;
    ivec3 map_pos = clamp(ivec3(floor(position)), ivec3(0), volume_size - 1);
    ivec3 ray_step = ivec3(sign(ray.dir));
    vec3 safe_dir = vec3(
        abs(ray.dir.x) < 0.000001 ? 0.000001 : ray.dir.x,
        abs(ray.dir.y) < 0.000001 ? 0.000001 : ray.dir.y,
        abs(ray.dir.z) < 0.000001 ? 0.000001 : ray.dir.z
    );
    vec3 delta_dist = abs(1.0 / safe_dir);
    vec3 next_boundary = vec3(map_pos) + step(vec3(0.0), ray.dir);
    vec3 side_dist = abs((next_boundary - position) / safe_dir);
    vec3 normal = entry_normal;
    float distance_travelled = entry;

    for (int step_index = 0; step_index < MAX_RAY_STEPS; step_index++) {
        if (all(greaterThanEqual(map_pos, ivec3(0))) && all(lessThan(map_pos, volume_size))) {
            if (texelFetch(voxel_texture, map_pos, 0).r != 0u) {
                hit_normal = normal;
                hit_distance = distance_travelled;
                return true;
            }
        }

        bvec3 mask = lessThanEqual(side_dist, min(side_dist.yzx, side_dist.zxy));
        float next_distance = min(side_dist.x, min(side_dist.y, side_dist.z));
        distance_travelled = entry + next_distance;
        side_dist += vec3(mask) * delta_dist;
        map_pos += ivec3(vec3(mask)) * ray_step;
        normal = -vec3(mask) * vec3(ray_step);

        if (distance_travelled > exit) {
            break;
        }
    }

    return false;
}

void main() {
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = vec2(pix) / vec2(imageSize(out_image));
    Ray ray = generate_ray(uv);
    vec3 color = vec3(0.02, 0.025, 0.04);
    vec3 hit_normal;
    float hit_distance;

    if (trace_voxels(ray, hit_normal, hit_distance)) {
        vec3 light_direction = normalize(vec3(-0.5, 0.8, -0.6));
        float diffuse = max(dot(hit_normal, light_direction), 0.0);
        color = vec3(0.25, 0.7, 0.35) * (0.2 + diffuse * 0.8);
    }

    imageStore(out_image, pix, vec4(color, 1.0));
}
