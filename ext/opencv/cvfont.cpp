/************************************************************

   cvfont.cpp -

   $Author: lsxi $

   Copyright (C) 2005-2006 Masakazu Yonekura

************************************************************/
#include "cvfont.h"
/*
 * Document-class: OpenCV::CvFont
 *
 * Font structure that can be passed to text rendering functions. 
 * see CvMat#put_text, CvMat#put_text!
 */

__NAMESPACE_BEGIN_OPENCV
__NAMESPACE_BEGIN_CVFONT

VALUE rb_klass;

int
rb_font_option_line_type(VALUE font_option)
{
  VALUE line_type = LOOKUP_HASH(font_option, "line_type");
  if (FIXNUM_P(line_type)) {
    return FIX2INT(line_type);
  }
  else if (line_type == ID2SYM(rb_intern("aa"))) {
    return CV_AA;
  }
  return 0;
}

VALUE
rb_class()
{
  return rb_klass;
}

VALUE
rb_allocate(VALUE klass)
{
  CvFont *ptr;
  return Data_Make_Struct(klass, CvFont, 0, -1, ptr);
}

/*
 * Create font object
 * @overload new(face, font_option = nil)
 * @param face [Symbol] Font name identifier. Only a subset of Hershey fonts (http://sources.isc.org/utils/misc/hershey-font.txt) are supported now:
 *   - :simplex - normal size sans-serif font
 *   - :plain - small size sans-serif font
 *   - :duplex - normal size sans-serif font (more complex than :simplex)
 *   - :complex - normal size serif font
 *   - :triplex - normal size serif font (more complex than :complex)
 *   - :complex_small - smaller version of :complex
 *   - :script_simplex - hand-writing style font
 *   - :script_complex - more complex variant of :script_simplex
 *
 * @param font_option [Hash] should be Hash include these keys.
 * @option font_option [Number] :hscale Horizontal scale. If equal to 1.0, the characters have the original width depending on the font type. If equal to 0.5, the characters are of half the original width.
 * @option font_option [Number] :vscale Vertical scale. If equal to 1.0, the characters have the original height depending on the font type. If equal to 0.5, the characters are of half the original height.
 * @option font_option [Number] :shear Approximate tangent of the character slope relative to the vertical line. Zero value means a non-italic font, 1.0f means ~45 degree slope, etc.
 * @option font_option [Number] :thickness Thickness of the text strokes.
 * @option font_option [Number] :line_type Type of the strokes, see CvMat#Line description.
 * @option font_option [Number] :italic If value is not nil or false that means italic or oblique font.
 *
 * @example Create Font
 *   OpenCV::CvFont.new(:simplex, :hscale => 2, :vslace => 2, :italic => true)
 *   # create 2x bigger than normal, italic type font.
 *
 * @opencv_func cvInitFont
 */
VALUE
rb_initialize(int argc, VALUE *argv, VALUE self)
{
  VALUE face, font_option;
  rb_scan_args(argc, argv, "11", &face, &font_option);
  Check_Type(face, T_SYMBOL);
  face = rb_hash_lookup(rb_const_get(cCvFont::rb_class(), rb_intern("FACE")), face);
  if (NIL_P(face)) {
    rb_raise(rb_eArgError, "undefined face.");
  }
  font_option = FONT_OPTION(font_option);

  int font_face = NUM2INT(face);
  if (FO_ITALIC(font_option)) {
    font_face |= CV_FONT_ITALIC;
  }
  try {
    cvInitFont(CVFONT(self),
	       font_face,
	       FO_HSCALE(font_option),
	       FO_VSCALE(font_option),
	       FO_SHEAR(font_option),
	       FO_THICKNESS(font_option),
	       FO_LINE_TYPE(font_option));
  }
  catch (cv::Exception& e) {
    raise_cverror(e);
  }

  return self;
}

/*
 * Returns font face
 * @overload face
 * @return [Integer] Font face
 */
VALUE
rb_face(VALUE self)
{
  return INT2FIX(CVFONT(self)->font_face);
}

/*
 * Returns hscale
 * @overload hscale
 * @return [Number] hscale
 */
VALUE
rb_hscale(VALUE self)
{
  return rb_float_new(CVFONT(self)->hscale);
}

/*
 * Returns vscale
 * @overload vscale
 * @return [Number] vscale
 */
VALUE
rb_vscale(VALUE self)
{
  return rb_float_new(CVFONT(self)->vscale);
}

/*
 * Returns shear
 * @overload shear
 * @return [Number] shear
 */
VALUE
rb_shear(VALUE self)
{
  return rb_float_new(CVFONT(self)->shear);
}

/*
 * Returns thickness
 * @overload thickness
 * @return [Integer] thickness
 */
VALUE
rb_thickness(VALUE self)
{
  return INT2FIX(CVFONT(self)->thickness);
}

/*
 * Returns line type
 * @overload line_type
 * @return [Integer] line_type
 */
VALUE
rb_line_type(VALUE self)
{
  return INT2FIX(CVFONT(self)->line_type);
}

/*
 * Returns italic or not
 * @overload italic
 * @return [Boolean] self is italic or not
 */
VALUE
rb_italic(VALUE self)
{
  return ((CVFONT(self)->font_face & CV_FONT_ITALIC) > 0) ? Qtrue : Qfalse;
}

void
init_ruby_class()
{
#if 0
  // For documentation using YARD
  VALUE opencv = rb_define_module("OpenCV");
#endif

  if (rb_klass)
    return;

  VALUE opencv = rb_module_opencv();
  rb_klass = rb_define_class_under(opencv, "CvFont", rb_cObject);
  rb_define_alloc_func(rb_klass, rb_allocate);
  VALUE face = rb_hash_new();
  rb_define_const(rb_klass, "FACE", face);
  rb_hash_aset(face, ID2SYM(rb_intern("simplex")), INT2FIX(CV_FONT_HERSHEY_SIMPLEX));
  rb_hash_aset(face, ID2SYM(rb_intern("plain")), INT2FIX(CV_FONT_HERSHEY_PLAIN));
  rb_hash_aset(face, ID2SYM(rb_intern("duplex")), INT2FIX(CV_FONT_HERSHEY_DUPLEX));
  rb_hash_aset(face, ID2SYM(rb_intern("triplex")), INT2FIX(CV_FONT_HERSHEY_TRIPLEX));
  rb_hash_aset(face, ID2SYM(rb_intern("complex_small")), INT2FIX(CV_FONT_HERSHEY_COMPLEX_SMALL));
  rb_hash_aset(face, ID2SYM(rb_intern("script_simplex")), INT2FIX(CV_FONT_HERSHEY_SCRIPT_SIMPLEX));
  rb_hash_aset(face, ID2SYM(rb_intern("script_complex")), INT2FIX(CV_FONT_HERSHEY_SCRIPT_COMPLEX));

  VALUE default_option = rb_hash_new();
  rb_define_const(rb_klass, "FONT_OPTION", default_option);
  rb_hash_aset(default_option, ID2SYM(rb_intern("hscale")), rb_float_new(1.0));
  rb_hash_aset(default_option, ID2SYM(rb_intern("vscale")), rb_float_new(1.0));
  rb_hash_aset(default_option, ID2SYM(rb_intern("shear")), INT2FIX(0));
  rb_hash_aset(default_option, ID2SYM(rb_intern("thickness")), INT2FIX(1));
  rb_hash_aset(default_option, ID2SYM(rb_intern("line_type")), INT2FIX(8));
  
  rb_define_method(rb_klass, "initialize", RUBY_METHOD_FUNC(rb_initialize), -1);
  rb_define_method(rb_klass, "face", RUBY_METHOD_FUNC(rb_face), 0);
  rb_define_method(rb_klass, "hscale", RUBY_METHOD_FUNC(rb_hscale), 0);
  rb_define_method(rb_klass, "vscale", RUBY_METHOD_FUNC(rb_vscale), 0);
  rb_define_method(rb_klass, "shear", RUBY_METHOD_FUNC(rb_shear), 0);
  rb_define_method(rb_klass, "thickness", RUBY_METHOD_FUNC(rb_thickness), 0);
  rb_define_method(rb_klass, "line_type", RUBY_METHOD_FUNC(rb_line_type), 0);
  rb_define_method(rb_klass, "italic", RUBY_METHOD_FUNC(rb_italic), 0);
}

__NAMESPACE_END_CVFONT
__NAMESPACE_END_OPENCV
