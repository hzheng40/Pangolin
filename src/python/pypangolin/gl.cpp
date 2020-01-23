#include "gl.hpp"
#include <pangolin/gl/gl.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "colour.hpp"
#include <pangolin/image/image_io.h>


namespace py_pangolin {

    bool is_packed(const pybind11::buffer_info & info) {
      int next_expected_stride = info.itemsize;
      for (int i = info.ndim-1; i >= 0; --i) {
        if (!(info.strides[i] == next_expected_stride)) {
          return false;
        }
        next_expected_stride *= info.shape[i];
      }
      return true;
    }

    void bind_gl(pybind11::module & m) {

        pybind11::class_<pangolin::GlTexture, std::shared_ptr<pangolin::GlTexture>>(m, "GlTexture")
            .def(pybind11::init<>(), "Default constructor, represents 'no texture'")
            .def(pybind11::init<GLint, GLint, GLint, bool, int, GLenum, GLenum, GLvoid*>(),
                pybind11::arg("width"), 
                pybind11::arg("height"), 
                pybind11::arg("internal_format") = GL_RGBA8, 
                pybind11::arg("sampling_linear") = true, 
                pybind11::arg("border") = 0,
                pybind11::arg("glformat") = GL_RGBA, 
                pybind11::arg("gltype") = GL_UNSIGNED_BYTE, 
                pybind11::arg("data") = nullptr,
                "internal_format normally one of GL_RGBA8, GL_LUMINANCE8, GL_INTENSITY16")

            .def("Reinitialise", 
                &pangolin::GlTexture::Reinitialise, 
                pybind11::arg("width"), 
                pybind11::arg("height"), 
                pybind11::arg("internal_format") = GL_RGBA8, 
                pybind11::arg("sampling_linear") = true, 
                pybind11::arg("border") = 0, 
                pybind11::arg("glformat") = GL_RGBA, 
                pybind11::arg("gltype") = GL_UNSIGNED_BYTE, 
                pybind11::arg("data") = nullptr, 
                "Reinitialise teture width / height / format")  // virtual function

            .def("IsValid", 
                &pangolin::GlTexture::IsValid)

            .def("Delete", 
                &pangolin::GlTexture::Delete, 
                "Delete OpenGL resources and fall back to representing 'no texture'")

            .def("Bind", 
                &pangolin::GlTexture::Bind)

            .def("Unbind", 
                &pangolin::GlTexture::Unbind)

            .def("Upload", 
                (void (pangolin::GlTexture::*) (const void*, GLenum, GLenum)) 
                &pangolin::GlTexture::Upload,
                pybind11::arg("image"), 
                pybind11::arg("data_format") = GL_LUMINANCE, 
                pybind11::arg("data_type") = GL_FLOAT,
                "data_layout normally one of GL_LUMINANCE, GL_RGB, ...\n"
                "data_type normally one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_FLOAT")

            .def("Upload", 
                (void (pangolin::GlTexture::*) (const void*, GLsizei, GLsizei, GLsizei, GLsizei, GLenum, GLenum))
                &pangolin::GlTexture::Upload, 
                pybind11::arg("data"), 
                pybind11::arg("tex_x_offset"),  
                pybind11::arg("tex_y_offset"), 
                pybind11::arg("data_w"), 
                pybind11::arg("data_h"),
                pybind11::arg("data_format"), 
                pybind11::arg("data_type"), 
                "Upload data to texture, overwriting a sub-region of it.\n"
                "data ptr contains packed data_w x data_h of pixel data.")

            .def("Download", 
                (void (pangolin::GlTexture::*)(void*, GLenum, GLenum) const) 
                &pangolin::GlTexture::Download, 
                pybind11::arg("image"), 
                pybind11::arg("data_layout") = GL_LUMINANCE, 
                pybind11::arg("data_type") = GL_FLOAT)

            .def("Download", 
                (void (pangolin::GlTexture::*)(pangolin::TypedImage&) const) 
                &pangolin::GlTexture::Download, 
                pybind11::arg("image"))

            .def("Upload", 
                [](pangolin::GlTexture &t, pybind11::array_t<unsigned char> img, GLenum data_format, GLenum data_type) {
                    auto buf = img.request();
                    unsigned char* data = (unsigned char*) buf.ptr;
                    t.Upload(data, data_format, data_type);
                },
                pybind11::arg("image"), 
                pybind11::arg("data_format") = GL_RGB, 
                pybind11::arg("data_type") = GL_UNSIGNED_BYTE)

            .def("Load", 
                &pangolin::GlTexture::Load, 
                pybind11::arg("image"), 
                pybind11::arg("sampling_linear") = true)

            .def("LoadFromFile", 
                &pangolin::GlTexture::LoadFromFile, 
                pybind11::arg("filename"), 
                pybind11::arg("sampling_linear"))

            .def("Save", 
                &pangolin::GlTexture::Save, 
                pybind11::arg("filename"), 
                pybind11::arg("top_line_first") = true)

            .def("SetLinear", 
                &pangolin::GlTexture::SetLinear)

            .def("SetNearestNeighbour", 
                &pangolin::GlTexture::SetNearestNeighbour)

            .def("RenderToViewport", 
                (void (pangolin::GlTexture::*) () const) &pangolin::GlTexture::RenderToViewport)

            .def("RenderToViewport", 
                (void (pangolin::GlTexture::*) (pangolin::Viewport, bool, bool) const) &pangolin::GlTexture::RenderToViewport,
                pybind11::arg("tex_vp"), 
                pybind11::arg("flipx") = false, 
                pybind11::arg("flipy") = false)

            .def("RenderToViewport", 
                (void (pangolin::GlTexture::*) (const bool) const) &pangolin::GlTexture::RenderToViewport, 
                pybind11::arg("flip"))

            .def("RenderToViewportFlipY", 
                &pangolin::GlTexture::RenderToViewportFlipY)

            .def("RenderToViewportFlipXFlipY", 
                &pangolin::GlTexture::RenderToViewportFlipXFlipY)

            .def_readwrite("internal_format", &pangolin::GlTexture::internal_format)

            .def_readwrite("tid", &pangolin::GlTexture::tid)

            .def_readwrite("width", &pangolin::GlTexture::width)

            .def_readwrite("height", &pangolin::GlTexture::height)

        ;

        pybind11::class_<pangolin::GlRenderBuffer>(m, "GlRenderBuffer")
        .def(pybind11::init<GLint, GLint, GLint>(), pybind11::arg("width")=0, pybind11::arg("height")=0, pybind11::arg("internal_format") = GL_DEPTH_COMPONENT24)
        .def("Reinitialise", &pangolin::GlRenderBuffer::Reinitialise);

      pybind11::class_<pangolin::GlFramebuffer>(m, "GlFramebuffer")
        .def(pybind11::init<pangolin::GlTexture &, pangolin::GlRenderBuffer &>())
        .def("Bind", &pangolin::GlFramebuffer::Bind)
        .def("Unbind", &pangolin::GlFramebuffer::Unbind);

      pybind11::enum_<pangolin::GlBufferType>(m, "GlBufferType")
        .value("GlUndefined", pangolin::GlBufferType::GlUndefined)
        .value("GlArrayBuffer", pangolin::GlBufferType::GlArrayBuffer)
        .value("GlElementArrayBuffer", pangolin::GlBufferType::GlElementArrayBuffer)
  #ifndef HAVE_GLES
        .value("GlPixelPackBuffer", pangolin::GlBufferType::GlPixelPackBuffer)
        .value("GlPixelUnpackBuffer", pangolin::GlBufferType::GlPixelUnpackBuffer)
        .value("GlShaderStorageBuffer", pangolin::GlBufferType::GlShaderStorageBuffer)
  #endif
        .export_values();

      pybind11::class_<pangolin::GlBufferData>(m, "GlBufferData")
        .def(pybind11::init<>())
        .def("Reinitialise", [](pangolin::GlBufferData & gl_buffer, pangolin::GlBufferType buffer_type, GLuint size_bytes, GLenum gl_use) {
          gl_buffer.Reinitialise(buffer_type, size_bytes, gl_use);
        })
        .def("Bind", &pangolin::GlBufferData::Bind)
        .def("Unbind", &pangolin::GlBufferData::Unbind)
        .def("Upload", [](pangolin::GlBufferData & gl_buffer, pybind11::buffer b, GLsizeiptr size_bytes, GLintptr offset) {
          pybind11::buffer_info info = b.request();
          gl_buffer.Upload(info.ptr, size_bytes, offset);
        }, pybind11::arg("data"), pybind11::arg("size_bytes"), pybind11::arg("offset")=0)
        .def_readwrite("size_bytes", &pangolin::GlBufferData::size_bytes);
      }

}  // py_pangolin