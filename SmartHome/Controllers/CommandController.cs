using Newtonsoft.Json;
using SmartHome.Models;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
//using System.Web.Http;
using System.Web.Mvc;
using uPLibrary.Networking.M2Mqtt;

namespace SmartHome.Controllers
{
    public class CommandController : Controller
    {
        //private IRootObject _root;

        private static ConcurrentDictionary<string, string> _commands = new ConcurrentDictionary<string, string>();

        private static MqttClient _mqtt;

        private static object _sync = new object();

        public CommandController()
        {            

            if (_mqtt == null)
            {
                lock(_sync)
                {
                    if (_mqtt == null)
                    {
                        _mqtt = new MqttClient(ConfigurationManager.AppSettings["mqtt:host"], Convert.ToInt32(ConfigurationManager.AppSettings["mqtt:port"]), false,null,null,MqttSslProtocols.None);
                        _mqtt.ConnectionClosed += MqttReconnect;
                        MqttReconnect(null, null);
                        //_mqtt.Connect("my_unique_identifier", ConfigurationManager.AppSettings["mqtt:user"], ConfigurationManager.AppSettings["mqtt:password"]);
                    }
                }
            }
            
        }

        private void MqttReconnect(object sender, EventArgs e)
        {
            _mqtt.Connect("my_unique_identifier", ConfigurationManager.AppSettings["mqtt:user"], ConfigurationManager.AppSettings["mqtt:password"]);
        }

        [HttpPost]        
        public void SendCommand(string command, string target)
        {
            //var cobj = _root.FindCompositeObject(target);
            //if (cobj == null) return;

            //_commands.AddOrUpdate(target, command, (k, v) => command);

            //if (_mqtt.IsConnected)
            //{
            //    //var cmd = String.Format("{{\"command\":\"{0}\", \"target\":\"{1}\"}}", command, target);
            //    //_mqtt.Publish("command/" + target.Replace("_", "/"), Encoding.ASCII.GetBytes(cmd));

            //    var cmd = JsonConvert.SerializeObject(new { Command = command, Target = cobj.HardId });

            //    _mqtt.Publish("command", Encoding.ASCII.GetBytes(cmd));
            //}
        }

        [HttpPost]
        public string RecieveCommand(string target)
        {
            string result = null;

            _commands.TryRemove(target, out result);

            return result;
        }

        [HttpPost]
        public void SetSensorValue(string sensorId, string val)
        {
            //var sensor = _root.FindSensor(sensorId);
            //if (sensor == null)
            //    return;

            //sensor.Value = val;
            //sensor.MeasureTime = DateTime.UtcNow.AddHours(3);

            //if (_mqtt.IsConnected)
            //{
            //    var msg = JsonConvert.SerializeObject(sensor);
            //    _mqtt.Publish("sensor", Encoding.ASCII.GetBytes(msg));
            //}


        }
    }
}
