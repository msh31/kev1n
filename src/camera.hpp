#pragma once

class CCamera {
  public:
      CCamera();

      Camera2D get() {
          return m_camera;
      }

      void update();

      int get_zoom_mode() { return m_zoom_mode; }
  private:
      Camera2D m_camera;
      int m_zoom_mode = 0; // 0-Mouse Wheel, 1-Mouse Move
      float m_zoom = 1.0f;
};
