// Copyright (c) 2017 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//

#ifndef CEF_INCLUDE_CAPI_CEF_RENDER_PROCESS_HANDLER_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_RENDER_PROCESS_HANDLER_CAPI_H_
#pragma once

#include "include/capi/cef_base_capi.h"
#include "include/capi/cef_browser_capi.h"
#include "include/capi/cef_dom_capi.h"
#include "include/capi/cef_frame_capi.h"
#include "include/capi/cef_load_handler_capi.h"
#include "include/capi/cef_process_message_capi.h"
#include "include/capi/cef_v8_capi.h"
#include "include/capi/cef_values_capi.h"

#ifdef __cplusplus
extern "C" {
#endif


///
// Structure used to implement render process callbacks. The functions of this
// structure will be called on the render process main thread (TID_RENDERER)
// unless otherwise indicated.
///
typedef struct _cef_render_process_handler_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Called after the render process main thread has been created. |extra_info|
  // is a read-only value originating from
  // cef_browser_process_handler_t::on_render_process_thread_created(). Do not
  // keep a reference to |extra_info| outside of this function.
  ///
  void (CEF_CALLBACK *on_render_thread_created)(
      struct _cef_render_process_handler_t* self,
      struct _cef_list_value_t* extra_info);

  ///
  // Called after WebKit has been initialized.
  ///
  void (CEF_CALLBACK *on_web_kit_initialized)(
      struct _cef_render_process_handler_t* self);

  ///
  // Called after a browser has been created. When browsing cross-origin a new
  // browser will be created before the old browser with the same identifier is
  // destroyed.
  ///
  void (CEF_CALLBACK *on_browser_created)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser);

  ///
  // Called before a browser is destroyed.
  ///
  void (CEF_CALLBACK *on_browser_destroyed)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser);

  ///
  // Return the handler for browser load status events.
  ///
  struct _cef_load_handler_t* (CEF_CALLBACK *get_load_handler)(
      struct _cef_render_process_handler_t* self);

  ///
  // Called before browser navigation. Return true (1) to cancel the navigation
  // or false (0) to allow the navigation to proceed. The |request| object
  // cannot be modified in this callback.
  ///
  int (CEF_CALLBACK *on_before_navigation)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser, struct _cef_frame_t* frame,
      struct _cef_request_t* request, cef_navigation_type_t navigation_type,
      int is_redirect);

  ///
  // Called immediately after the V8 context for a frame has been created. To
  // retrieve the JavaScript 'window' object use the
  // cef_v8context_t::get_global() function. V8 handles can only be accessed
  // from the thread on which they are created. A task runner for posting tasks
  // on the associated thread can be retrieved via the
  // cef_v8context_t::get_task_runner() function.
  ///
  void (CEF_CALLBACK *on_context_created)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser, struct _cef_frame_t* frame,
      struct _cef_v8context_t* context);

  ///
  // Called immediately before the V8 context for a frame is released. No
  // references to the context should be kept after this function is called.
  ///
  void (CEF_CALLBACK *on_context_released)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser, struct _cef_frame_t* frame,
      struct _cef_v8context_t* context);

  ///
  // Called for global uncaught exceptions in a frame. Execution of this
  // callback is disabled by default. To enable set
  // CefSettings.uncaught_exception_stack_size > 0.
  ///
  void (CEF_CALLBACK *on_uncaught_exception)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser, struct _cef_frame_t* frame,
      struct _cef_v8context_t* context, struct _cef_v8exception_t* exception,
      struct _cef_v8stack_trace_t* stackTrace);

  ///
  // Called when a new node in the the browser gets focus. The |node| value may
  // be NULL if no specific node has gained focus. The node object passed to
  // this function represents a snapshot of the DOM at the time this function is
  // executed. DOM objects are only valid for the scope of this function. Do not
  // keep references to or attempt to access any DOM objects outside the scope
  // of this function.
  ///
  void (CEF_CALLBACK *on_focused_node_changed)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser, struct _cef_frame_t* frame,
      struct _cef_domnode_t* node);

  ///
  // Called when a new message is received from a different process. Return true
  // (1) if the message was handled or false (0) otherwise. Do not keep a
  // reference to or attempt to access the message outside of this callback.
  ///
  int (CEF_CALLBACK *on_process_message_received)(
      struct _cef_render_process_handler_t* self,
      struct _cef_browser_t* browser, cef_process_id_t source_process,
      struct _cef_process_message_t* message);
} cef_render_process_handler_t;


#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_RENDER_PROCESS_HANDLER_CAPI_H_
