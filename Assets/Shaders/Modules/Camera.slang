module camera;

public struct CameraData {
    public matrix view_projection;
    public matrix view_projection_no_camera_position;
    public float3 up;
    public float3 right;
};