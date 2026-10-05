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

public class MainActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        // Code
        super.onCreate(savedInstanceState);

        MyTextView myTextView = new MyTextView(this);

        getWindow().getDecorView().setBackgroundColor(Color.BLACK);

        setContentView(myTextView);
    }
}
