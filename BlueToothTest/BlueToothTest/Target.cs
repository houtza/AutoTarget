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
		public ImageView View { get; set; }
		public TextView DisplayHitCounter { get; set; }
		public bool TargetPosition { get; set; }  //up = true, down = false
		public int HitCount { get; set; }

		//public Target(TextView targetView)
		//{
		//	View = targetView;
		//	TargetPosition = false;
		//}


		//public void Retract()
		//{
		//	TargetPosition = false;

		//	View.SetBackgroundColor(Color.Red);
		//	HitCount = HitCount + 1;
		//	View.Text = HitCount.ToString();

		//}

		//public void Extend()
		//{
		//	TargetPosition = true;
		//	View.SetBackgroundColor(Color.Green);
		//}

		public Target(ImageView targetView, TextView hitView)
		{
			View = targetView;
			DisplayHitCounter = hitView;//--------
			TargetPosition = false;
		}


		public void TargetHitBullsEye()
		{
			HitCount = HitCount + 1;
			int resourceId = (int)typeof(Resource.Drawable).GetField("targetHitCenter").GetValue(null);
			View.SetImageResource(resourceId);
			DisplayHitCounter.Text = String.Concat("Hit count: ", HitCount);//---
		}

		public void TargetHitOuterRing()
		{
			HitCount = HitCount + 1;
			int resourceId = (int)typeof(Resource.Drawable).GetField("targetHitOuter").GetValue(null);
			View.SetImageResource(resourceId);
			DisplayHitCounter.Text = String.Concat("Hit count: ", HitCount);//---
		}

		public void Retract()
		{
			TargetPosition = false;

			//View.SetBackgroundColor(Color.Red);
			//HitCount = HitCount + 1;
			//View.Text = HitCount.ToString();
			int resourceId = (int)typeof(Resource.Drawable).GetField("targetRetracted").GetValue(null);
			View.SetImageResource(resourceId);

		}

		public void Extend()
		{
			TargetPosition = true;
			//View.SetBackgroundColor(Color.Green);
			int resourceId = (int)typeof(Resource.Drawable).GetField("targetUp").GetValue(null);
			View.SetImageResource(resourceId);
		}

	}
}