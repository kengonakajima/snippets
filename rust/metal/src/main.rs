use objc2::rc::Retained;
use objc2::runtime::ProtocolObject;
use objc2::{MainThreadMarker, MainThreadOnly};
use objc2_app_kit::{
    NSApplication, NSApplicationActivationPolicy, NSBackingStoreType, NSWindow,
    NSWindowStyleMask,
};
use objc2_foundation::{ns_string, NSPoint, NSRect, NSSize, NSString};
use objc2_metal::{
    MTLClearColor, MTLCommandBuffer, MTLCommandEncoder, MTLCommandQueue, MTLDevice, MTLDrawable,
    MTLLibrary, MTLPixelFormat, MTLPrimitiveType, MTLRenderCommandEncoder, MTLRenderPassDescriptor,
    MTLRenderPipelineDescriptor, MTLRenderPipelineState, MTLResourceOptions, MTLVertexDescriptor,
    MTLVertexFormat,
};
use objc2_quartz_core::{CAMetalDrawable, CAMetalLayer};
use std::ptr::NonNull;
use std::time::Instant;

#[repr(C)]
struct Vertex {
    position: [f32; 2],
    color: [f32; 4],
}

#[repr(C)]
struct Uniforms {
    angle: f32,
}

fn main() {
    let mtm = MainThreadMarker::new().expect("Must run on main thread");

    let app = NSApplication::sharedApplication(mtm);
    app.setActivationPolicy(NSApplicationActivationPolicy::Regular);

    let window = unsafe {
        let style = NSWindowStyleMask::Titled
            | NSWindowStyleMask::Closable
            | NSWindowStyleMask::Resizable
            | NSWindowStyleMask::Miniaturizable;

        let frame = NSRect::new(NSPoint::new(100.0, 100.0), NSSize::new(800.0, 600.0));

        NSWindow::initWithContentRect_styleMask_backing_defer(
            NSWindow::alloc(mtm),
            frame,
            style,
            NSBackingStoreType::Buffered,
            false,
        )
    };

    window.setTitle(ns_string!("Metal Triangle Demo"));
    window.makeKeyAndOrderFront(None);
    window.center();

    let device = objc2_metal::MTLCreateSystemDefaultDevice().expect("No Metal device found");

    let metal_layer = CAMetalLayer::new();
    metal_layer.setDevice(Some(&device));
    metal_layer.setPixelFormat(MTLPixelFormat::BGRA8Unorm);
    metal_layer.setFramebufferOnly(true);

    let content_view = window.contentView().expect("No content view");
    unsafe {
        let layer_ptr: *const CAMetalLayer = &*metal_layer;
        content_view.setLayer(Some(&*(layer_ptr as *const objc2_quartz_core::CALayer)));
    }
    content_view.setWantsLayer(true);

    let frame = content_view.frame();
    metal_layer.setDrawableSize(objc2_core_foundation::CGSize::new(
        frame.size.width,
        frame.size.height,
    ));

    let library = create_library(&device);
    let pipeline_state = create_pipeline(&device, &library);
    let vertex_buffer = create_vertex_buffer(&device);
    let command_queue = device.newCommandQueue().expect("Failed to create command queue");

    let start_time = Instant::now();

    #[allow(deprecated)]
    app.activateIgnoringOtherApps(true);

    loop {
        let pool = unsafe { objc2_foundation::NSAutoreleasePool::new() };

        loop {
            let event: Option<Retained<objc2_app_kit::NSEvent>> = unsafe {
                app.nextEventMatchingMask_untilDate_inMode_dequeue(
                    objc2_app_kit::NSEventMask::Any,
                    None,
                    objc2_foundation::NSDefaultRunLoopMode,
                    true,
                )
            };
            match event {
                Some(e) => {
                    app.sendEvent(&e);
                }
                None => break,
            }
        }

        if !window.isVisible() {
            break;
        }

        let elapsed = start_time.elapsed().as_secs_f32();
        render(
            &metal_layer,
            &pipeline_state,
            &vertex_buffer,
            &command_queue,
            elapsed,
        );

        drop(pool);
    }
}

fn create_library(
    device: &ProtocolObject<dyn MTLDevice>,
) -> Retained<ProtocolObject<dyn MTLLibrary>> {
    let shader_src = include_str!("../shaders.metal");
    let source = NSString::from_str(shader_src);
    let library = device
        .newLibraryWithSource_options_error(&source, None)
        .expect("Failed to compile shader library");
    library
}

fn create_pipeline(
    device: &ProtocolObject<dyn MTLDevice>,
    library: &ProtocolObject<dyn MTLLibrary>,
) -> Retained<ProtocolObject<dyn MTLRenderPipelineState>> {
    let vertex_fn = library
        .newFunctionWithName(ns_string!("vertex_main"))
        .expect("Vertex function not found");
    let fragment_fn = library
        .newFunctionWithName(ns_string!("fragment_main"))
        .expect("Fragment function not found");

    let descriptor = MTLRenderPipelineDescriptor::new();
    descriptor.setVertexFunction(Some(&vertex_fn));
    descriptor.setFragmentFunction(Some(&fragment_fn));

    let vertex_descriptor = MTLVertexDescriptor::new();

    // Position attribute
    let pos_attr = unsafe { vertex_descriptor.attributes().objectAtIndexedSubscript(0) };
    pos_attr.setFormat(MTLVertexFormat::Float2);
    unsafe {
        pos_attr.setOffset(0);
        pos_attr.setBufferIndex(0);
    }

    // Color attribute
    let color_attr = unsafe { vertex_descriptor.attributes().objectAtIndexedSubscript(1) };
    color_attr.setFormat(MTLVertexFormat::Float4);
    unsafe {
        color_attr.setOffset(8); // 2 * sizeof(f32)
        color_attr.setBufferIndex(0);
    }

    // Layout
    let layout = unsafe { vertex_descriptor.layouts().objectAtIndexedSubscript(0) };
    unsafe {
        layout.setStride(24); // sizeof(Vertex) = 2*4 + 4*4 = 24
    }
    layout.setStepFunction(objc2_metal::MTLVertexStepFunction::PerVertex);

    descriptor.setVertexDescriptor(Some(&vertex_descriptor));

    let color_attachments = descriptor.colorAttachments();
    let attachment = unsafe { color_attachments.objectAtIndexedSubscript(0) };
    attachment.setPixelFormat(MTLPixelFormat::BGRA8Unorm);

    device
        .newRenderPipelineStateWithDescriptor_error(&descriptor)
        .expect("Failed to create pipeline state")
}

fn create_vertex_buffer(
    device: &ProtocolObject<dyn MTLDevice>,
) -> Retained<ProtocolObject<dyn objc2_metal::MTLBuffer>> {
    let vertices: [Vertex; 3] = [
        Vertex {
            position: [0.0, 0.5],
            color: [1.0, 0.0, 0.0, 1.0], // Red
        },
        Vertex {
            position: [-0.5, -0.5],
            color: [0.0, 1.0, 0.0, 1.0], // Green
        },
        Vertex {
            position: [0.5, -0.5],
            color: [0.0, 0.0, 1.0, 1.0], // Blue
        },
    ];

    let buffer = unsafe {
        device
            .newBufferWithBytes_length_options(
                NonNull::new(vertices.as_ptr() as *mut _).unwrap(),
                std::mem::size_of_val(&vertices),
                MTLResourceOptions::CPUCacheModeDefaultCache,
            )
            .expect("Failed to create vertex buffer")
    };

    buffer
}

fn render(
    layer: &CAMetalLayer,
    pipeline_state: &ProtocolObject<dyn MTLRenderPipelineState>,
    vertex_buffer: &ProtocolObject<dyn objc2_metal::MTLBuffer>,
    command_queue: &ProtocolObject<dyn MTLCommandQueue>,
    time: f32,
) {
    let drawable = match layer.nextDrawable() {
        Some(d) => d,
        None => return,
    };

    let pass_descriptor = MTLRenderPassDescriptor::new();
    let color_attachments = pass_descriptor.colorAttachments();
    let attachment = unsafe { color_attachments.objectAtIndexedSubscript(0) };

    attachment.setTexture(Some(&drawable.texture()));
    attachment.setLoadAction(objc2_metal::MTLLoadAction::Clear);
    attachment.setStoreAction(objc2_metal::MTLStoreAction::Store);
    attachment.setClearColor(MTLClearColor {
        red: 0.1,
        green: 0.1,
        blue: 0.1,
        alpha: 1.0,
    });

    let command_buffer = command_queue
        .commandBuffer()
        .expect("Failed to create command buffer");

    let encoder = command_buffer
        .renderCommandEncoderWithDescriptor(&pass_descriptor)
        .expect("Failed to create render encoder");

    encoder.setRenderPipelineState(pipeline_state);
    unsafe {
        encoder.setVertexBuffer_offset_atIndex(Some(vertex_buffer), 0, 0);
    }

    let uniforms = Uniforms { angle: time };
    let uniforms_ptr = NonNull::new(&uniforms as *const _ as *mut _).unwrap();
    unsafe {
        encoder.setVertexBytes_length_atIndex(uniforms_ptr, std::mem::size_of::<Uniforms>(), 1);
    }

    unsafe {
        encoder.drawPrimitives_vertexStart_vertexCount(MTLPrimitiveType::Triangle, 0, 3);
    }

    encoder.endEncoding();

    let drawable_obj: &ProtocolObject<dyn MTLDrawable> = ProtocolObject::from_ref(&*drawable);
    command_buffer.presentDrawable(drawable_obj);
    command_buffer.commit();
}
