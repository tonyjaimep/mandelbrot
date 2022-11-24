#ifdef GL_ES
precision mediump float
#endif

#define MAX_ITERATIONS 200.0

uniform vec2 resolution;
uniform vec2 min_displayed;
uniform vec2 max_displayed;

vec2 range = max_displayed - min_displayed;

float mandelbrot(vec2 initial_position) {
  float iteration = 0.0;
  vec2 position = vec2(initial_position);

  while (
    ((position.x * position.x) + (position.y * position.y) < 4.0)
    && (iteration < MAX_ITERATIONS)
  ) {
    position = vec2(
      position.x * position.x - position.y * position.y + initial_position.x,
      2.0 * position.x * position.y + initial_position.y
    ),
    iteration++;
  }

  return iteration;
}

void main() {
  vec2 st = gl_FragCoord.st / resolution;

  float scaled_x = (st.x * range.x + min_displayed.x);
  float scaled_y = (st.y * range.y + min_displayed.y);
  float m = mandelbrot(vec2(scaled_x, scaled_y));
  float v = m / MAX_ITERATIONS;

  gl_FragColor = vec4(
    v,
    v,
    v,
    1.0
  );
}
