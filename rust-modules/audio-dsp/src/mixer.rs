/// Audio mixer for combining multiple audio channels
pub struct AudioMixer {
    channel_count: usize,
    channel_volumes: Vec<f32>,
}

impl AudioMixer {
    /// Create a new audio mixer with the specified number of channels
    pub fn new(channel_count: usize) -> Self {
        Self {
            channel_count,
            channel_volumes: vec![1.0; channel_count], // Default volume is 1.0 (100%)
        }
    }

    /// Get the number of channels
    pub fn channel_count(&self) -> usize {
        self.channel_count
    }

    /// Mix multiple audio channels into a single output buffer
    ///
    /// # Arguments
    /// * `input_buffers` - Slice of input channel buffers
    /// * `output_buffer` - Output buffer to write mixed audio
    pub fn mix(&self, input_buffers: &[&[f32]], output_buffer: &mut [f32]) {
        let frame_count = output_buffer.len();

        // Clear output buffer
        output_buffer.fill(0.0);

        // Mix each channel with its volume
        for (channel_idx, input_buffer) in input_buffers.iter().enumerate() {
            if channel_idx >= self.channel_count {
                break;
            }

            let volume = self.channel_volumes[channel_idx];

            for (frame_idx, sample) in input_buffer.iter().take(frame_count).enumerate() {
                output_buffer[frame_idx] += sample * volume;
            }
        }

        // Simple clipping to prevent overflow
        for sample in output_buffer.iter_mut() {
            *sample = sample.clamp(-1.0, 1.0);
        }
    }

    /// Set volume for a specific channel (0.0 to 1.0+)
    /// Returns true if successful, false if channel index is invalid
    pub fn set_channel_volume(&mut self, channel: usize, volume: f32) -> bool {
        if channel >= self.channel_count {
            return false;
        }

        self.channel_volumes[channel] = volume.max(0.0); // Ensure non-negative
        true
    }

    /// Get volume for a specific channel
    pub fn get_channel_volume(&self, channel: usize) -> Option<f32> {
        self.channel_volumes.get(channel).copied()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_mixer_creation() {
        let mixer = AudioMixer::new(2);
        assert_eq!(mixer.channel_count(), 2);
        assert_eq!(mixer.get_channel_volume(0), Some(1.0));
        assert_eq!(mixer.get_channel_volume(1), Some(1.0));
    }

    #[test]
    fn test_basic_mixing() {
        let mixer = AudioMixer::new(2);
        let input1 = vec![0.5, 0.5, 0.5];
        let input2 = vec![0.3, 0.3, 0.3];
        let inputs = vec![input1.as_slice(), input2.as_slice()];
        let mut output = vec![0.0; 3];

        mixer.mix(&inputs, &mut output);

        // Expected: 0.5 + 0.3 = 0.8
        assert_eq!(output, vec![0.8, 0.8, 0.8]);
    }

    #[test]
    fn test_volume_control() {
        let mut mixer = AudioMixer::new(2);
        mixer.set_channel_volume(0, 0.5);
        mixer.set_channel_volume(1, 0.0);

        let input1 = vec![1.0, 1.0];
        let input2 = vec![1.0, 1.0];
        let inputs = vec![input1.as_slice(), input2.as_slice()];
        let mut output = vec![0.0; 2];

        mixer.mix(&inputs, &mut output);

        // Expected: 1.0 * 0.5 + 1.0 * 0.0 = 0.5
        assert_eq!(output, vec![0.5, 0.5]);
    }

    #[test]
    fn test_clipping() {
        let mixer = AudioMixer::new(2);
        let input1 = vec![1.0, 1.0];
        let input2 = vec![1.0, 1.0];
        let inputs = vec![input1.as_slice(), input2.as_slice()];
        let mut output = vec![0.0; 2];

        mixer.mix(&inputs, &mut output);

        // Expected: clipped to 1.0
        assert_eq!(output, vec![1.0, 1.0]);
    }
}
