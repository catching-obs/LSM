use crossbeam::channel::{bounded, Sender, Receiver};
use std::sync::atomic::{AtomicBool, AtomicU64, Ordering};
use std::sync::Arc;
use std::thread;

#[derive(Debug, Clone)]
pub struct EncoderConfig {
    pub width: u32,
    pub height: u32,
    pub fps: u32,
    pub bitrate: u32,
}

#[derive(Debug, Clone)]
pub struct FrameData {
    pub data: Vec<u8>,
    pub timestamp: i64,
}

pub struct EncoderPipeline {
    config: EncoderConfig,
    frame_sender: Option<Sender<FrameData>>,
    frame_receiver: Option<Receiver<FrameData>>,
    running: Arc<AtomicBool>,
    encoded_frames: Arc<AtomicU64>,
    worker_handle: Option<thread::JoinHandle<()>>,
}

impl EncoderPipeline {
    /// Create a new encoder pipeline with the given configuration
    pub fn new(config: EncoderConfig) -> Result<Self, String> {
        let (sender, receiver) = bounded(64); // Buffer up to 64 frames

        Ok(Self {
            config,
            frame_sender: Some(sender),
            frame_receiver: Some(receiver),
            running: Arc::new(AtomicBool::new(false)),
            encoded_frames: Arc::new(AtomicU64::new(0)),
            worker_handle: None,
        })
    }

    /// Submit a frame for encoding
    pub fn submit_frame(&mut self, frame: FrameData) -> Result<(), String> {
        if !self.running.load(Ordering::Relaxed) {
            return Err("Encoder not running".to_string());
        }

        if let Some(ref sender) = self.frame_sender {
            sender.send(frame).map_err(|e| e.to_string())?;
            Ok(())
        } else {
            Err("Encoder channel not available".to_string())
        }
    }

    /// Start the encoding pipeline worker thread
    pub fn start(&mut self) -> Result<(), String> {
        if self.running.load(Ordering::Relaxed) {
            return Err("Encoder already running".to_string());
        }

        let receiver = self.frame_receiver
            .take()
            .ok_or("Receiver already taken")?;

        let running = Arc::clone(&self.running);
        let encoded_frames = Arc::clone(&self.encoded_frames);
        let config = self.config.clone();

        running.store(true, Ordering::Relaxed);

        // Spawn worker thread
        let handle = thread::spawn(move || {
            Self::worker_loop(receiver, running, encoded_frames, config);
        });

        self.worker_handle = Some(handle);
        Ok(())
    }

    /// Stop the encoding pipeline
    pub fn stop(&mut self) -> Result<(), String> {
        if !self.running.load(Ordering::Relaxed) {
            return Ok(());
        }

        self.running.store(false, Ordering::Relaxed);

        // Drop sender to signal worker thread to finish
        self.frame_sender = None;

        if let Some(handle) = self.worker_handle.take() {
            handle.join().map_err(|_| "Failed to join worker thread")?;
        }

        Ok(())
    }

    /// Get the number of encoded frames
    pub fn get_encoded_frames(&self) -> u64 {
        self.encoded_frames.load(Ordering::Relaxed)
    }

    /// Worker thread loop for encoding frames
    fn worker_loop(
        receiver: Receiver<FrameData>,
        running: Arc<AtomicBool>,
        encoded_frames: Arc<AtomicU64>,
        config: EncoderConfig,
    ) {
        while running.load(Ordering::Relaxed) {
            match receiver.recv_timeout(std::time::Duration::from_millis(100)) {
                Ok(frame) => {
                    // Simulate encoding work
                    Self::encode_frame(&frame, &config);
                    encoded_frames.fetch_add(1, Ordering::Relaxed);
                }
                Err(crossbeam::channel::RecvTimeoutError::Timeout) => {
                    // Continue waiting
                    continue;
                }
                Err(crossbeam::channel::RecvTimeoutError::Disconnected) => {
                    // Channel closed, exit loop
                    break;
                }
            }
        }
    }

    /// Encode a single frame (placeholder implementation)
    fn encode_frame(frame: &FrameData, config: &EncoderConfig) {
        // TODO: Implement actual encoding using ffmpeg or other encoder
        // For now, this is a placeholder that simulates work

        // Simulate encoding delay
        std::thread::sleep(std::time::Duration::from_millis(5));

        // Log encoding progress (in production, this would actually encode)
        #[cfg(debug_assertions)]
        eprintln!(
            "Encoding frame: timestamp={}, size={} bytes, config={}x{}@{}fps",
            frame.timestamp,
            frame.data.len(),
            config.width,
            config.height,
            config.fps
        );
    }
}

impl Drop for EncoderPipeline {
    fn drop(&mut self) {
        let _ = self.stop();
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_encoder_creation() {
        let config = EncoderConfig {
            width: 1920,
            height: 1080,
            fps: 30,
            bitrate: 5000000,
        };

        let pipeline = EncoderPipeline::new(config);
        assert!(pipeline.is_ok());
    }

    #[test]
    fn test_encoder_start_stop() {
        let config = EncoderConfig {
            width: 1920,
            height: 1080,
            fps: 30,
            bitrate: 5000000,
        };

        let mut pipeline = EncoderPipeline::new(config).unwrap();
        assert!(pipeline.start().is_ok());
        assert!(pipeline.stop().is_ok());
    }

    #[test]
    fn test_frame_submission() {
        let config = EncoderConfig {
            width: 1920,
            height: 1080,
            fps: 30,
            bitrate: 5000000,
        };

        let mut pipeline = EncoderPipeline::new(config).unwrap();
        pipeline.start().unwrap();

        let frame = FrameData {
            data: vec![0u8; 1920 * 1080 * 3],
            timestamp: 0,
        };

        assert!(pipeline.submit_frame(frame).is_ok());

        std::thread::sleep(std::time::Duration::from_millis(100));
        assert!(pipeline.get_encoded_frames() > 0);

        pipeline.stop().unwrap();
    }
}
