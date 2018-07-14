package tools.mbot.domurad.mbotsimpleandroidvideoclient;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Display;
import android.view.TextureView;

import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {


    VideoPlayer player;
    Timer timer;

    int screenWidth;
    int screenHeight;

    int screenSegmentWidth;
    int screenSegmentHeight;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);

        screenWidth = size.x;
        screenHeight = size.y;

        screenSegmentWidth = screenWidth/3;
        screenSegmentHeight = screenHeight/3;

        TextureView tv = findViewById(R.id.textureView);

        tv.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                player = new VideoPlayer(surface, "192.168.0.17",10200,800,600);
                player.start();
            }

            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            }

            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                return false;
            }

            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {

            }
        });
    }

    @Override
    public void onResume(){
        super.onResume();

        if(player != null)
                  player.start();

        setupErrorTimer();
    }

    @Override
    public void onPause(){
        timer.cancel();
        super.onPause();
        player.stop();
    }

    private void setupErrorTimer() {
        timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                if(player == null)
                    return;

                if(player.hasError()){
                    String errorsStr = "";
                    for(String err : player.errors())
                        errorsStr += "ERROR: " + err + "\n";

                    final String finalErrorsStr = errorsStr;
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            timer.cancel();
                            AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(MainActivity.this);
                            dlgAlert.setMessage(finalErrorsStr);
                            dlgAlert.setTitle("Application Error");

                            dlgAlert.setPositiveButton("Ok",
                                    new DialogInterface.OnClickListener() {
                                        public void onClick(DialogInterface dialog, int which) {
                                            MainActivity.this.finishAffinity();
                                            System.exit(0);
                                        }
                                    });

                            dlgAlert.setCancelable(true);
                            dlgAlert.create().show();
                        }
                    });
                }
            }
        }, 1000, 1000);
    }
}
