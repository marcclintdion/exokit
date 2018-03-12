#ifndef _EXOKIT_BINDINGS_H_
#define _EXOKIT_BINDINGS_H_

#include <v8.h>
#include <node.h>
#include <image-context.h>
#include <imageData-context.h>
#include <imageBitmap-context.h>
#include <canvas-context.h>
#include <path2d-context.h>
#include <glfw.h>
#include <webgl.h>
#include <AudioContext.h>
#include <Video.h>

class WebGLContext : public ObjectWrap {
public:
  static Handle<Object> Initialize(Isolate *isolate);

protected:
  WebGLContext();
  ~WebGLContext();

  static NAN_METHOD(New);
  static NAN_METHOD(GetWindowHandle);
  static NAN_METHOD(SetWindowHandle);

  GLFWwindow *windowHandle;

  template<NAN_METHOD(F)>
  static NAN_METHOD(setContextWrap) {
    {
      Nan::HandleScope scope;

      Local<Object> glObj = info.This();
      WebGLContext *gl = ObjectWrap::Unwrap<WebGLContext>(glObj);
      if (gl->windowHandle) {
        glfw::SetCurrentWindowContext(gl->windowHandle);
      }
    }

    F(info);
  }
};

v8::Local<v8::Object> makeGl();
v8::Local<v8::Object> makeImage();
v8::Local<v8::Object> makeImageData();
v8::Local<v8::Object> makeImageBitmap(Local<Value> imageCons);
v8::Local<v8::Object> makeCanvasRenderingContext2D(Local<Value> imageDataCons);
v8::Local<v8::Object> makePath2D();
v8::Local<v8::Object> makeAudio();
v8::Local<v8::Object> makeVideo();

#endif
