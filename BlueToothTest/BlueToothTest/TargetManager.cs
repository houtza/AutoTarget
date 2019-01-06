using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Android.App;
using Android.Content;
using Android.OS;
using Android.Runtime;
using Android.Views;
using Android.Widget;

namespace BlueToothTest
{
	public class TargetManager
	{
		public List<Target> TargetList = new List<Target>();
		public void SetTargets(byte data)
		{
			
			int targetNumber = data >> 4;
			Target tempTarget = TargetList[targetNumber-1];
			tempTarget.Actuate();
			

			
		}

	}
}