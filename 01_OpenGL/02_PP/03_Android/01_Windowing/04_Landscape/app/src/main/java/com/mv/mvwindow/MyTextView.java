//
//  MyTextView.java
//  
//
//  Created by Mayur Tadekar on 17/11/25.
//
package com.mv.mvwindow;

import android.view.Gravity;
import android.graphics.Color;
import android.content.Context;
import androidx.appcompat.widget.AppCompatTextView;

public class MyTextView extends AppCompatTextView
{
    public MyTextView(Context context)
    {
        // Code
        super(context);
        
        setTextColor(Color.rgb(0, 255, 0));
        setTextSize(60);
        setGravity(Gravity.CENTER);
        setText("Hello, World!!!");
    }
}
