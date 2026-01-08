mod mixer;

pub use mixer::AudioMixer;

use std::os::raw::c_float;

/// FFI interface for audio mixing
/// Creates a new audio mixer with specified channel count
#[no_mangle]
pub extern "C" fn audio_mixer_create(channel_count: usize) -> *mut AudioMixer {
    let mixer = Box::new(AudioMixer::new(channel_count));
    Box::into_raw(mixer)
}

/// Destroys an audio mixer instance
#[no_mangle]
pub extern "C" fn audio_mixer_destroy(mixer: *mut AudioMixer) {
    if !mixer.is_null() {
        unsafe {
            drop(Box::from_raw(mixer));
        }
    }
}

/// Mix audio channels
#[no_mangle]
pub extern "C" fn audio_mixer_mix(
    mixer: *mut AudioMixer,
    input_buffers: *const *const c_float,
    output_buffer: *mut c_float,
    frame_count: usize,
) -> bool {
    if mixer.is_null() || input_buffers.is_null() || output_buffer.is_null() {
        return false;
    }

    unsafe {
        let mixer_ref = &mut *mixer;
        let channel_count = mixer_ref.channel_count();

        // Convert raw pointers to slices
        let mut input_slices = Vec::with_capacity(channel_count);
        for i in 0..channel_count {
            let buffer_ptr = *input_buffers.add(i);
            if buffer_ptr.is_null() {
                return false;
            }
            input_slices.push(std::slice::from_raw_parts(buffer_ptr, frame_count));
        }

        let output_slice = std::slice::from_raw_parts_mut(output_buffer, frame_count);

        mixer_ref.mix(&input_slices, output_slice);
    }

    true
}

/// Set volume for a specific channel
#[no_mangle]
pub extern "C" fn audio_mixer_set_channel_volume(
    mixer: *mut AudioMixer,
    channel: usize,
    volume: c_float,
) -> bool {
    if mixer.is_null() {
        return false;
    }

    unsafe {
        let mixer_ref = &mut *mixer;
        mixer_ref.set_channel_volume(channel, volume)
    }
}
