#include "camera.hpp"
#include "globals.hpp"

CCamera::CCamera( ) {
    m_camera = { 0 };
    m_camera.zoom = m_zoom;
    m_camera.offset = { g_window_width / 2.0f, g_window_height / 2.0f };
}

void CCamera::update( ) {
    if ( IsKeyPressed( KEY_ONE ) ) m_zoom_mode = 0;
    else if ( IsKeyPressed( KEY_TWO ) )
        m_zoom_mode = 1;

    if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT ) ) {
        Vector2 delta = GetMouseDelta( );
        delta = Vector2Scale( delta, -1.0f / m_camera.zoom );
        m_camera.target = Vector2Add( m_camera.target, delta );
    }

    if ( m_zoom_mode == 0 ) {
        float wheel = GetMouseWheelMove( );
        if ( wheel != 0 ) {
            Vector2 mouseWorldPos = GetScreenToWorld2D( GetMousePosition( ), m_camera );

            m_camera.offset = GetMousePosition( );
            m_camera.target = mouseWorldPos;

            float scale = 0.2f * wheel;
            m_camera.zoom = Clamp( expf( logf( m_camera.zoom ) + scale ), 0.125f, 64.0f );
        }
    } else {
        if ( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT ) ) {
            Vector2 mouseWorldPos = GetScreenToWorld2D( GetMousePosition( ), m_camera );

            m_camera.offset = GetMousePosition( );
            m_camera.target = mouseWorldPos;
        }

        if ( IsMouseButtonDown( MOUSE_BUTTON_RIGHT ) ) {
            float deltaX = GetMouseDelta( ).x;
            float scale = 0.005f * deltaX;
            m_camera.zoom = Clamp( expf( logf( m_camera.zoom ) + scale ), 0.125f, 64.0f );
        }
    }
}
