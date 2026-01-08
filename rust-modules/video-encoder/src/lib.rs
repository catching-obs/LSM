mod pipeline;

pub use pipeline::{EncoderPipeline, EncoderConfig, FrameData};

use std::os::raw::{c_int, c_void};

/// FFI interface for video encoder

#[repr(C)]
pub struct CEncoderConfig {
    pub width: c_int,
    pub height: c_int,
    pub fps: c_int,
    pub bitrate: c_int,
}

/// Create a new encoder pipeline
#[no_mangle]
pub extern "C" fn encoder_create(config: CEncoderConfig) -> *mut EncoderPipeline {
    let encoder_config = EncoderConfig {
        width: config.width as u32,
        height: config.height as u32,
        fps: config.fps as u32,
        bitrate: config.bitrate as u32,
    };

    match EncoderPipeline::new(encoder_config) {
        Ok(pipeline) => Box::into_raw(Box::new(pipeline)),
        Err(_) => std::ptr::null_mut(),
    }
}

/// Destroy an encoder pipeline
#[no_mangle]
pub extern "C" fn encoder_destroy(encoder: *mut EncoderPipeline) {
    if !encoder.is_null() {
        unsafe {
            drop(Box::from_raw(encoder));
        }
    }
}

/// Submit a frame for encoding
#[no_mangle]
pub extern "C" fn encoder_submit_frame(
    encoder: *mut EncoderPipeline,
    frame_data: *const c_void,
    frame_size: usize,
    timestamp: i64,
) -> bool {
    if encoder.is_null() || frame_data.is_null() {
        return false;
    }

    unsafe {
        let encoder_ref = &mut *encoder;
        let data_slice = std::slice::from_raw_parts(frame_data as *const u8, frame_size);
        let frame = FrameData {
            data: data_slice.to_vec(),
            timestamp,
        };

        encoder_ref.submit_frame(frame).is_ok()
    }
}

/// Start the encoding pipeline
#[no_mangle]
pub extern "C" fn encoder_start(encoder: *mut EncoderPipeline) -> bool {
    if encoder.is_null() {
        return false;
    }

    unsafe {
        let encoder_ref = &mut *encoder;
        encoder_ref.start().is_ok()
    }
}

/// Stop the encoding pipeline
#[no_mangle]
pub extern "C" fn encoder_stop(encoder: *mut EncoderPipeline) -> bool {
    if encoder.is_null() {
        return false;
    }

    unsafe {
        let encoder_ref = &mut *encoder;
        encoder_ref.stop().is_ok()
    }
}

/// Get encoding statistics
#[no_mangle]
pub extern "C" fn encoder_get_encoded_frames(encoder: *const EncoderPipeline) -> u64 {
    if encoder.is_null() {
        return 0;
    }

    unsafe {
        let encoder_ref = &*encoder;
        encoder_ref.get_encoded_frames()
    }
}
