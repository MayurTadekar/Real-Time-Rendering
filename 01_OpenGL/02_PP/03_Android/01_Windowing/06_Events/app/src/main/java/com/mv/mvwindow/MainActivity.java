//
//  MainActivity.java
//  
//
//  Created by Mayur Tadekar on 17/11/25.
//
package com.mv.mvwindow;

import android.os.Bundle;
import android.graphics.Color;
import androidx.appcompat.app.AppCompatActivity;

// Packages for Fullscreen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

// Orientation Related
import android.content.pm.ActivityInfo;

public class MainActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // Code
        super.onCreate(savedInstanceState);

        // Forced Landscape Orientation
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        
        // Fullscreen
        getSupportActionBar().hide();
        
        // Tell the android system to make your window expand edge to edge of phone
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        
        // Get Window Insets Controller
        WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
        
        // The insets controller to hide remaining bar and insets
        windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());
        
        MyTextView myTextView = new MyTextView(this);

        getWindow().getDecorView().setBackgroundColor(Color.BLACK);

        setContentView(myTextView);
    }
}
