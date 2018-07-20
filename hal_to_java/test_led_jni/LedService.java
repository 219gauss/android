
package com.android.server;
import android.os.ILedService;


public class LedService extends ILedService.Stub
{

	public LedService(){
		openLed();
	}

	public void SetLed(int no,int val){
		set_Led(no,val);
	}
	
	private static native boolean openLed();
	private static native int set_Led(int no,int val);
}








