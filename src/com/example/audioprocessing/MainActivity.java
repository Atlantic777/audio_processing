package com.example.audioprocessing;

import java.io.IOException;
import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

import android.media.MediaPlayer;
import android.media.audiofx.Equalizer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.PowerManager;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ActionBar.LayoutParams;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnDismissListener;
import android.content.res.AssetFileDescriptor;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;
import com.example.audioprocessing.VisualizerView;
import com.example.audioprocessing.AudioRecordingHandler;
import com.example.audioprocessing.AudioRecordingHandler;
import com.example.audioprocessing.BarGraphRenderer;
import com.example.audioprocessing.FFT;
import com.example.audioprocessing.Renderer;
import com.example.audioprocessing.PlaybackHandler;

@SuppressLint({ "ShowToast", "NewApi" })
public class MainActivity extends Activity {

	Thread t;
	public static final int FIR = 0;
	Button stop = null;
	int threadFir = 0;
	int checkedFir = 0;
	int stopFir = 0;
	int stopIir= 0;
	Timer timerFir;
	Handler handlerFir;
	TimerTask timerTaskFir;
	VisualizerView visualizerView;
	AudioPlaybackManager playbackManagerFir;
	Context mainActivity;
	

	private PlaybackHandler playbackHandlerFir = new PlaybackHandler() {
		@Override
		public void onPreparePlayback() {
			runOnUiThread(new Runnable() {
				public void run() {
					playbackManagerFir.showMediaController();
				}
			});
		}
	};


	static {
		System.loadLibrary("audio-jni");
	}

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		final ProcessDialog dialogFir = new ProcessDialog(this);
		dialogFir.setTitle("Processing");
		final CheckBox checkBox = (CheckBox) findViewById(R.id.checkBox1);
		final Button fir = (Button) findViewById(R.id.button1);
		visualizerView = (VisualizerView) findViewById(R.id.visualizerView);
		playbackManagerFir = new AudioPlaybackManager(visualizerView.mainActivity,
				visualizerView, playbackHandlerFir);
		playbackManagerFir = new AudioPlaybackManager(
				visualizerView.mainActivity,
				visualizerView, playbackHandlerFir);
		timerFir = new Timer();
		handlerFir = new Handler();
		
		fir.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				
				
				dialogFir.setContentView(R.layout.dialog);
				final TextView myText = (TextView) dialogFir.findViewById(R.id.timeText);
				dialogFir.setCanceledOnTouchOutside(false);
				dialogFir.show();
					
								threadFir = 1;
								t = new Thread(new Runnable() {
									@Override
									public void run() {
										
										timerTaskFir = new TimerTask() {
											int countFir = 0;
											@Override
											public void run() {
												handlerFir.post(new Runnable() { 
													public void run() {
														
														runOnUiThread(new Runnable() {
														
															@Override
															public void run() {
																myText.setText(""+(countFir++) + " sec");
															}
														});
													}
												});
											}
										};
										
										timerFir.scheduleAtFixedRate(timerTaskFir,0, 1000L);
										audioProcessingJNI(FIR); // 0 - fir filter
										dialogFir.cancel();
										if (checkBox.isChecked() && stopFir == 0) {
											checkedFir = 1;
											setupVisualizer();
											runOnUiThread(new Runnable() {
												
												@Override
												public void run() {
													playbackManagerFir.setupPlayback(getString(R.string._sdcard_outfir_wav));
													playbackManagerFir.showMediaController();
												}
											});
										}else if(stopFir == 1) {
											stopFir = 0;
										}
									}
								});
							t.start();
			}
		});
	}

	public static native void audioProcessingJNI(int choice);
	public static native void stopProcessingJNI();

	private void setupVisualizer() {
		Paint paint = new Paint();
		paint.setStrokeWidth(5f); // set bar width
		paint.setAntiAlias(true);
		paint.setColor(Color.argb(200, 227, 69, 53)); // set bar color
		BarGraphRenderer barGraphRendererBottom = new BarGraphRenderer(2,
				paint, false);
		visualizerView.addRenderer(barGraphRendererBottom);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		if (playbackManagerFir.isPlaying()) {
			playbackManagerFir.showMediaController();
		}
		return false;
	}

	@Override
	protected void onDestroy() {
		if(checkedFir == 1) {
			try {
				t.join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			playbackManagerFir.dispose();
			playbackHandlerFir = null;
			checkedFir = 0;
		}
		
			
		if(threadFir == 1) {
			 if(timerFir != null) {
				 timerFir.cancel();
				 timerFir = null;
			 }
			threadFir = 0;
		}
		
		super.onDestroy();
	}
	 
	 private class ProcessDialog extends Dialog {

		public ProcessDialog(Context context) {
			super(context);
			setContentView(R.layout.dialog);
		}
		
		@Override
		public boolean onKeyUp(int keyCode, KeyEvent event) {
			 super.onKeyUp(keyCode, event);
			 if(keyCode == KeyEvent.KEYCODE_ESCAPE) {
					
				 if(threadFir == 1) {
					 stopFir = 1;
					 stopProcessingJNI();
					 try {
						t.join();
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					 threadFir = 0;
				 }
			 }
			return true;
		}
    }
}

