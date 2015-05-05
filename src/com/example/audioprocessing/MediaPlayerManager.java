package com.example.audioprocessing;

import java.io.FileInputStream;

import android.media.MediaPlayer;
import android.util.Log;
import android.view.SurfaceHolder;

/*
* Manages media player playback
*/

public class MediaPlayerManager {

private MediaPlayer player;

public MediaPlayerManager() {
player = new MediaPlayer();
}

public MediaPlayer getPlayer() {
return player;
}

public void setupPlayback(String fileName) {
try {
	FileInputStream fileInputStream = new FileInputStream(fileName);
	player.setDataSource(fileInputStream.getFD());         
	fileInputStream.close();
            player.prepare();
            
        } catch (Exception e) {
            e.printStackTrace();
        }
}

public void setDisplay(SurfaceHolder sf) {
player.setDisplay(sf);
}

public void startPlaying() {
player.start();
    }

public void pausePlaying() {
player.pause();
    }

public void seekTo(int pos) {
if (pos < 0) {
player.seekTo(0);
}
else if (pos > getDuration()) {
player.seekTo(getDuration());
}
else {
player.seekTo(pos);
}
}

public void stopPlaying() {
if (player.isPlaying()) {
player.stop();
}	
    }

public boolean isPlaying() {
return player.isPlaying();
}

public int getCurrentPosition() {
return player.getCurrentPosition();
}

public int getDuration() {
return player.getDuration();
}

public void releasePlayer() {
setDisplay(null);
player.release();
player = null;
}
}
