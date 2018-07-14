package tools.mbot.domurad.mbotsimpleandroidvideoclient;

import android.graphics.SurfaceTexture;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.view.Surface;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class VideoPlayer implements Runnable{

    private final SurfaceTexture surfaceTexture;
    private final String hostAddress;
    private final int hostPort;
    private final int videoW;
    private final int videoH;

    private boolean isRunning;
    private boolean shouldRun;

    private Thread workerThread;

    private boolean playerError = false;
    private List<String> playerErrorStrings = new ArrayList<>();

    private synchronized void setError(String errStr){
        playerError = true;
        shouldRun = false;
        playerErrorStrings.add(errStr);
    }

    public VideoPlayer(SurfaceTexture surfaceTexture, String hostAddress, int hostPort, int videoW, int videoH) {
        this.surfaceTexture = surfaceTexture;
        this.hostAddress = hostAddress;
        this.hostPort = hostPort;
        this.videoW = videoW;
        this.videoH = videoH;
    }

    public boolean hasError(){
        return playerError;
    }

    public List<String> errors(){
        return playerErrorStrings;
    }

    public void start() {
        if(isRunning)
            return;

        isRunning = true;
        workerThread = new Thread(this);
        shouldRun = true;
        workerThread.start();
    }

    public void stop() {
        shouldRun = false;

        try {
            workerThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }finally {
            isRunning = false;
        }
    }

    public void run() {
        MediaCodec mediaCodec = null;
        Socket inputSocket = null;
        try {

            MediaFormat format = MediaFormat.createVideoFormat("video/avc", videoW, videoH);
            format.setByteBuffer("csd-0", ByteBuffer.wrap(new byte[]{0x00, 0x00, 0x00, 0x01, 0x27, 0x64, 0x00, 0x28, (byte)0xAC, 0x2B, 0x40, 0x64, 0x09, (byte)0xBF, 0x2C, 0x03, (byte)0xC4, (byte)0x89, (byte)0xA8}));
            format.setByteBuffer("csd-1", ByteBuffer.wrap(new byte[]{0x00, 0x00, 0x00, 0x01, 0x28, (byte)0xEE, 0x1F, 0x2C}));

            try {
                mediaCodec = MediaCodec.createDecoderByType("video/avc");
                mediaCodec.configure(format, new Surface(surfaceTexture), null,0 );
                mediaCodec.start();

            } catch (IOException e) {
                e.printStackTrace();
                setError(e.getMessage());
                return;
            }

            boolean readMetadata = true;
            int recSize = 0;
            int dataToRead = 0;
            int frameSize = 0;
            byte[] medatadata = new byte[4];
            byte[] frameData = new byte[1024];

            InetAddress inetAddress = InetAddress.getByName(hostAddress);
            inputSocket = new Socket(inetAddress, hostPort);
            InputStream inputStream = inputSocket.getInputStream();

            long startMs = System.currentTimeMillis();
            while(shouldRun) {
                if (readMetadata) {
                    readMetadata = false;

                    recSize = inputStream.read(medatadata, 0, 4);
                    if (recSize != 4) {
                        setError("Video stream END or protocol ERROR. (recSize != 4)");
                        return;
                    }

                    frameSize = java.nio.ByteBuffer.wrap(medatadata).order(java.nio.ByteOrder.LITTLE_ENDIAN).getInt();
                    dataToRead = frameSize;

                    if (frameSize > frameData.length)
                        frameData = new byte[frameSize];

                } else {
                    if (inputStream.available() < dataToRead) {
                        Thread.sleep(10);
                    } else {
                        recSize = inputStream.read(frameData, frameSize - dataToRead, dataToRead);
                        if (recSize <= 0) {
                            setError("Video stream END or protocol ERROR. (recSize <= 0)");
                            return;
                        }

                        dataToRead -= recSize;
                        if (dataToRead == 0) {
                            readMetadata = true;

                            int ibi = mediaCodec.dequeueInputBuffer(10000);
                            if (ibi >= 0) {
                                ByteBuffer ib = mediaCodec.getInputBuffers()[ibi];
                                ib.clear();
                                ib.put(frameData);

                                long pt = System.currentTimeMillis();
                                mediaCodec.queueInputBuffer(ibi, 0, frameSize, pt - startMs, 0);
                            }
                        }
                    }
                }


                MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
                int obi = mediaCodec.dequeueOutputBuffer(info, 10000);

                if (obi > 0) {
                    mediaCodec.releaseOutputBuffer(obi, true);
                }
            }

        } catch (UnknownHostException e) {
            e.printStackTrace();
            setError(e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
            setError(e.getMessage());
        } catch (InterruptedException e) {
            e.printStackTrace();
            setError(e.getMessage());
        }
        finally {
            if(mediaCodec != null)
                mediaCodec.stop();

            if(inputSocket != null)
                try {
                    inputSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                    setError(e.getMessage());
                }
        }
    }
}
