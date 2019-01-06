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
	public class MailBox
	{
		private byte _mail;

		public byte Mail
		{
			get
			{
				return _mail;
			}
			set
			{
				_mail = value;
				MailReceived(value);
			}
		}
		

		public event MailReceivedDelegate MailReceived;

		

	}
}