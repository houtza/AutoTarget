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


namespace BlueToothTest
{
    [Activity(Label = "@string/app_name", Theme = "@style/AppTheme.NoActionBar", MainLauncher = true)]
    public class MainActivity : AppCompatActivity
    {
        private String dataPast = null;
        private String data = null;
		enum Game { whackAmole, remoteControll };
		enum TargetState { waitForConnection, sendGameMode, gameMode };
		//public delegate void MailReceivedDelegate(byte dataReceived);
		public TargetStateManager targetStateManager = new TargetStateManager();
		public MailBox mailBox = new MailBox();
		public TargetManager targetManager = new TargetManager();

		protected override void OnCreate(Bundle savedInstanceState)
        {
			mailBox.MailReceived += OnMailReceived;

			base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.activity_main);

            Android.Support.V7.Widget.Toolbar toolbar = FindViewById<Android.Support.V7.Widget.Toolbar>(Resource.Id.toolbar);
            SetSupportActionBar(toolbar);

            FloatingActionButton fab = FindViewById<FloatingActionButton>(Resource.Id.fab);
            fab.Click += FabOnClick;

            BluetoothManager manager = new BluetoothManager();
            manager.getAllPairedDevices();

            TextView currentCharacterName = FindViewById<TextView>(Resource.Id.text);
            currentCharacterName.Text = "Your Text";

			var edtFirstText = FindViewById<EditText>(Resource.Id.edtFirstText);

			var btnDisplay = FindViewById<Button>(Resource.Id.btnDisplay);

			btnDisplay.Click += (e, o) =>
			{
				//currentCharacterName.Text = edtFirstText.Text;
				manager.sendDataToDevice("5");//----
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

			

        }

		//else if(targetStateManager.State == (int)TargetState.sendGameMode && (dataReceived & 0b00001111) == 0b00001111)
		public void OnMailReceived(byte dataReceived)
		{
			targetStateManager.updateState(dataReceived);
			if (targetStateManager.InGameMode)
			{
				targetManager.SetTargets(dataReceived);
			}
			else if (targetStateManager.State == (int)TargetState.sendGameMode && (dataReceived & 0b00001111) == 0b00001111)
			{
				DynamicalyPopulateTargets(2); //Dynamicaly generate target UI
				//Send Game mode
			}
			else if(targetStateManager.State == (int)TargetState.gameMode && dataReceived == 0b11111111)
			{
				targetStateManager.InGameMode = true;
			}
			else
			{
				//Corrupted
			}


				TextView currentCharacterName = FindViewById<TextView>(Resource.Id.text);
			currentCharacterName.Text = Convert.ToString(dataReceived, 2);
		}
		

		public void DynamicalyPopulateTargets(int targetToAdd)
		{
			LinearLayout layoutBase = FindViewById<LinearLayout>(Resource.Id.layoutBase);
			for(int i = 0; i < targetToAdd; i++)
			{
				
				var FakeTarget = new TextView(this);
				FakeTarget.Text = ("Target" + i.ToString());
				FakeTarget.Id = i + 1;

				Target target = new Target(FakeTarget);
				
				layoutBase.AddView(FakeTarget);

				targetManager.TargetList.Add(target);
			}
		}


        public override bool OnCreateOptionsMenu(IMenu menu)
        {
            MenuInflater.Inflate(Resource.Menu.menu_main, menu);
            return true;
        }

        public override bool OnOptionsItemSelected(IMenuItem item)
        {
            int id = item.ItemId;
            if (id == Resource.Id.action_settings)
            {
                return true;
            }

            return base.OnOptionsItemSelected(item);
        }

        private void FabOnClick(object sender, EventArgs eventArgs)
        {
            View view = (View) sender;
            Snackbar.Make(view, "Replace with your own action", Snackbar.LengthLong)
                .SetAction("Action", (Android.Views.View.IOnClickListener)null).Show();
        }
	}
}

