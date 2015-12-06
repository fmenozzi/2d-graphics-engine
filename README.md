An engine for producing 2D graphics as produced over the course of the semester in my university graphics course. All code
with the `Fed` prefix is mine, while everything else was provided by the instructor. Features of the engine include:

  * Blending 
  * Shading
  * Clipping
  * Bitmaps
  * Transformations
  * Convex Polygons
  * Gradients
  * Stroking (opaque colors only)

To buld, test, and produce sample images, use

    $ make
    $ ./tests
    $ ./image -e expected

(Requires `libpng` to build)

Sample images created with the engine are shown below:

![alt tag](results/solid_ramp.png)

![alt tag](results/blend_white.png)

![alt tag](results/blend_black.png)

![alt tag](results/circles_blend.png)

![alt tag](results/spocks_quad.png) 

![alt tag](results/spocks_zoom.png) 

![alt tag](results/circles_fat.png)

![alt tag](results/tri.png)

![alt tag](results/tri_clipped.png)

![alt tag](results/poly.png)

![alt tag](results/poly_center.png)

![alt tag](results/poly_rotate.png)

![alt tag](results/draw_concat_scale.png)

![alt tag](results/draw_concat_scale_bitmap.png)

![alt tag](results/draw_concat_rotate.png)

![alt tag](results/draw_concat_rotate_bitmap.png)

![alt tag](results/gradient_insets.png)

![alt tag](results/gradient_insets_rot.png)

![alt tag](results/linear_big.png)

![alt tag](results/radial.png)

![alt tag](results/radial_quad.png)

![alt tag](results/radial_big.png)

![alt tag](results/bitmap_shader_clamp.png)

![alt tag](results/poly_shaders.png)

![alt tag](results/stroke_line.png)

![alt tag](results/stroke_polyline.png)

![alt tag](results/stroke_rect.png)

![alt tag](results/stroke_rings.png)

![alt tag](results/stroke_star.png)
