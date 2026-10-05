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

// Package for Fullscreen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

public class MainActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // Code
        super.onCreate(savedInstanceState);
        
        // Fullscreen
        getSupportActionBar().hide();
        
        // Tell the android system to make your window expand edge to edge of phone
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        
        // Get Window Insets Controller
        WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
        
        windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());
        
        MyTextView myTextView = new MyTextView(this);
        
        getWindow().getDecorView().setBackgroundColor(Color.BLACK);
        
        setContentView(myTextView);
    }
}

