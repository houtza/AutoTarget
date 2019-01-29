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
    [Activity(Label = "@string/app_name", Theme = "@style/AppTheme.NoActionBar", MainLauncher = true)]
    public class MainActivity : AppCompatActivity
    {

		int gameMode = -1;
		private System.Timers.Timer _timer;

		public override void OnBackPressed()
		{
			return;
		}

		protected override void OnCreate(Bundle savedInstanceState)
        {
			

			base.OnCreate(savedInstanceState);
            SetContentView(Resource.Layout.content_main);


			

			var edtFirstText = FindViewById<EditText>(Resource.Id.edtFirstText);

			var startBtn = FindViewById<Button>(Resource.Id.startBtn);


			Spinner spinner = FindViewById<Spinner>(Resource.Id.spinner);

			spinner.ItemSelected += new EventHandler<AdapterView.ItemSelectedEventArgs>(spinner_ItemSelected);
			var adapter = ArrayAdapter.CreateFromResource(
					this, Resource.Array.game_prompt_array, Android.Resource.Layout.SimpleSpinnerItem);

			adapter.SetDropDownViewResource(Android.Resource.Layout.SimpleSpinnerDropDownItem);
			spinner.Adapter = adapter;

			startBtn.Click += (e, o) =>
			{

				if(gameMode != -1)
				{
					Intent targetActivity = new Intent(this, typeof(target_control_activity));
					targetActivity.PutExtra("gameMode", gameMode);
					StartActivity(targetActivity);
				}
				
			};

            			

        }


		private void spinner_ItemSelected(object sender, AdapterView.ItemSelectedEventArgs e)
		{

			Spinner spinner = (Spinner)sender;
			
			gameMode = e.Position;
			//string toast = string.Format("The planet is {0}", spinner.GetItemAtPosition(e.Position));
			//Toast.MakeText(this, toast, ToastLength.Long).Show();
		}










	}
}

