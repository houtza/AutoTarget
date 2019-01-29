using System;
//using System.Threading;
using Android.App;
using Android.OS;
using Android.Runtime;
using Android.Support.Design.Widget;
using Android.Support.V7.App;
using Android.Views;
using Android.Widget;
using System.Threading.Tasks;
using Android.Content;

namespace BlueToothTest
{
	[Activity(Label = "target_control_activity")]
	public class target_control_activity : Activity
	{
		int gameMode = 0;
		int targetsDeployed = 0;
		private String dataPast = null;
		private String data = null;
		public bool exceptDataFromESP = false;
		enum Game { whackAmole, remoteControll };
		enum TargetState { waitForConnection, sendGameMode, gameMode };
		//public delegate void MailReceivedDelegate(byte dataReceived);
		public TargetStateManager targetStateManager = new TargetStateManager();
		public MailBox mailBox = new MailBox();
		public TargetManager targetManager = new TargetManager();

		public override void OnBackPressed()
		{
			return;
		}

		protected override void OnCreate(Bundle savedInstanceState)
		{

			mailBox.MailReceived += OnMailReceived;
			base.OnCreate(savedInstanceState);

			// Create your application here

			SetContentView(Resource.Layout.target_control_activity);
			gameMode = Intent.GetIntExtra("gameMode", -1);


			BluetoothManager manager = new BluetoothManager();
			manager.getAllPairedDevices();

			TextView currentCharacterName = FindViewById<TextView>(Resource.Id.text);
			currentCharacterName.Text = "Your Text";

			var edtFirstText = FindViewById<EditText>(Resource.Id.edtFirstText);

			var btnDisplay = FindViewById<Button>(Resource.Id.btnDisplay);


			 

			btnDisplay.Click += (e, o) =>
			{
				
				exceptDataFromESP = true;
				//currentCharacterName.Text = gameMode.ToString();
				manager.sendDataToDevice(edtFirstText.Text);//----
			};

			System.Threading.Thread thread = new System.Threading.Thread(() =>
			{
				while (true)
				{
					int data = manager.getDataFromDevice();
					//String data = manager.getDataFromDevice();

					//Console.WriteLine(value);
					//RunOnUiThread(async () => { currentCharacterName.Text = data.ToString(); });
					//RunOnUiThread(async () => { currentCharacterName.Text = Convert.ToString(data,2); });

					RunOnUiThread(async () => { mailBox.Mail = (byte)data; });

				}
			});

			thread.IsBackground = true;
			thread.Start();



			exceptDataFromESP = true;
			manager.sendDataToDevice(Convert.ToString(15));
		}


		


		public void OnMailReceived(byte dataReceived)
		{
			if (exceptDataFromESP)
			{
				targetStateManager.updateState(dataReceived);
				if (targetStateManager.InGameMode)
				{
					targetManager.SetTargets(dataReceived);
				}
				else if (targetStateManager.State == (int)TargetState.sendGameMode && (dataReceived | 0b00001111) == 0b00001111)
				{
					Android.App.AlertDialog.Builder alertDialog = new Android.App.AlertDialog.Builder(this);
					alertDialog.SetTitle("No targets found");
					alertDialog.SetMessage("No targets found, please restart the app and try again");
					alertDialog.SetNeutralButton("OK", delegate {
						alertDialog.Dispose();
					});
					alertDialog.Show();
				}
				else if (targetStateManager.State == (int)TargetState.sendGameMode && (dataReceived & 0b00001111) == 0b00001111)// up to 15 targets
				{
					DynamicalyPopulateTargets((dataReceived >> 4)); //Dynamicaly generate target UI
					targetsDeployed = dataReceived >> 4;
				}
				else if (targetStateManager.State == (int)TargetState.gameMode && dataReceived == 0b11111111)
				{
					targetStateManager.InGameMode = true;
				}
				else
				{
					//Corrupted
				}
			}

			TextView currentCharacterName = FindViewById<TextView>(Resource.Id.text);
			currentCharacterName.Text = Convert.ToString(dataReceived, 2);
		}




		public void DynamicalyPopulateTargets(int targetToAdd)
		{
			LinearLayout layoutBase = FindViewById<LinearLayout>(Resource.Id.layoutBase);
			for (int i = 0; i < targetToAdd; i++)
			{

				var FakeTarget = new TextView(this);
				FakeTarget.Text = ("Target" + i.ToString());
				FakeTarget.Id = i + 1;

				Target target = new Target(FakeTarget);

				layoutBase.AddView(FakeTarget);

				targetManager.TargetList.Add(target);
			}
		}





		

		public void closeApplication()
		{
			//var activity = (Activity)Android.Context;
			//activity.FinishAffinity();
		}





	}
}