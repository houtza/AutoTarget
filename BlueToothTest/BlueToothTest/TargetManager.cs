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
			int hitLocation = (data & 0b00000110) >> 1;
			int targetPositionControll = (data & 0b00000001);

			Target tempTarget = TargetList[targetNumber-1];

			if (hitLocation > 0) 
			{
				if(hitLocation == 1)
				{
					tempTarget.TargetHitBullsEye();
				}
				else
				{
					tempTarget.TargetHitOuterRing();
				}
				//tempTarget.Retract();
			}
			else
			{
				if (targetPositionControll == 1) //Retract Target
				{
					tempTarget.Retract();
				}
				else
				{
					tempTarget.Extend();   //Raise Target
				}
			}
						
		}

	}
}


//int targetNumber = data >> 4;
//int hitLocation = (data & 0b00000111);  //int hitLocation = (data & 0b00000110) >> 1;

//Target tempTarget = TargetList[targetNumber - 1];

//			if (hitLocation > 0) 
//			{
//				tempTarget.Retract();
//			}
//			else
//			{
//				if (tempTarget.TargetPosition)
//				{
//					tempTarget.Retract();
//				}
//				else
//				{
//					tempTarget.Extend();
//				}
//			}