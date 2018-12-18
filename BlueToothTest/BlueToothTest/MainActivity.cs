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
        protected override void OnCreate(Bundle savedInstanceState)
        {
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
				currentCharacterName.Text = edtFirstText.Text;
			};

            System.Threading.Thread thread = new System.Threading.Thread(() =>
            {
                while (true)
                {
                    data = manager.getDataFromDevice();
                    //manager.sendDataToDevice("H");//--
                    
                    RunOnUiThread(async () => { currentCharacterName.Text = data; });
                }
            });

            thread.IsBackground = true;
            thread.Start();

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

