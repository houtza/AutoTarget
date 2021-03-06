﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.Graphics;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace BlueToothTest
{
	public class Target
	{

		public Target(TextView targetView)
		{
			View = targetView;
			TargetPosition = false;
		}
		public TextView View { get; set; }
		public bool TargetPosition { get; set; }  //up = true, down = false

		public int HitCount { get; set; }

		public void Retract()
		{
			TargetPosition = false;
			
			View.SetBackgroundColor(Color.Red);
			HitCount = HitCount + 1;
			View.Text = HitCount.ToString();
		
		}

		public void Extend()
		{
			TargetPosition = true;
			View.SetBackgroundColor(Color.Green);
		}
		
	}
}