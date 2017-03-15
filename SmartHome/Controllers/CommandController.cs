using log4net;
using Microsoft.AspNet.Identity;
using Microsoft.AspNet.Identity.Owin;
using Newtonsoft.Json;
using SmartHome.Models;
using SmartHome.Models.DataContracts;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Web;
//using System.Web.Http;
using System.Web.Mvc;
using uPLibrary.Networking.M2Mqtt;

namespace SmartHome.Controllers
{
    public class CommandController : Controller
    {
        //private IRootObject _root;

        private RootUnit _root;

        private static ConcurrentDictionary<string, string> _commands = new ConcurrentDictionary<string, string>();

        private static MqttClient _mqtt;

        private static object _sync = new object();

        private static ILog _logger = LogManager.GetLogger("CommandController");

        public CommandController()
        {
            var user = System.Web.HttpContext.Current.GetOwinContext().GetUserManager<SmartHomeUserManager>().FindByName(System.Web.HttpContext.Current.User.Identity.Name);

            if (user != null && user.Profiles.Any())
            {
                _root = user.Profiles.Where(p => p.Name == user.DefaultProfileName).FirstOrDefault();
                _root.UpdateParents();
            }            

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
            _mqtt.Connect("my_unique_identifier_" + DateTime.Now.Ticks.ToString(), ConfigurationManager.AppSettings["mqtt:user"], ConfigurationManager.AppSettings["mqtt:password"]);
            _mqtt.Subscribe(new string[] { "sensor" }, new byte[] { 0 });
            _mqtt.MqttMsgPublishReceived += (s, args) =>
            {
                var topic = args.Topic;
                var msg = Encoding.UTF8.GetString(args.Message);

                var dto = JsonConvert.DeserializeObject<SensorTO>(msg);

                SetSensorValue(dto.SensorId, dto.SensorValue);
            };
        }

        /// <summary>
        /// Послать команду
        /// </summary>
        /// <param name="command">Имя команды</param>
        /// <param name="target">ClientId объекта, содержащего команду</param>
        [HttpPost]
        public void SendCommand(string command, string target)
        {
            var unit = _root.FindUnit(target);
            if (unit == null) return;

            _commands.AddOrUpdate(target, command, (k, v) => command);

            if (!_mqtt.IsConnected)
            {
                MqttReconnect(null, null);
            }

            //var cmd = String.Format("{{\"command\":\"{0}\", \"target\":\"{1}\"}}", command, target);
            //_mqtt.Publish("command/" + target.Replace("_", "/"), Encoding.ASCII.GetBytes(cmd));

            var cmd = JsonConvert.SerializeObject(new { Command = command, Target = unit.Id });

            //var user = User.Identity.GetUserName();
            var manager = HttpContext.GetOwinContext().GetUserManager<SmartHomeUserManager>();

            SmartHomeUser user = System.Web.HttpContext.Current.GetOwinContext().GetUserManager<SmartHomeUserManager>().FindById(System.Web.HttpContext.Current.User.Identity.GetUserId());

            var topic = String.Format("{0}/{1}/{2}", user.UserName, user.DefaultProfileName, unit.ClientId.Replace("_", "/"));

            _mqtt.Publish(topic, Encoding.UTF8.GetBytes(cmd));

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
            _logger.DebugFormat("Start setting sensor {0} value to {1}", sensorId, val);

            var sensor = _root.FindSensor(sensorId);
            if (sensor == null)
            {
                _logger.DebugFormat("Sensor {0} not found", sensorId);
                return;
            }

            sensor.Value = val;
            sensor.MeasureTime = DateTime.UtcNow.AddHours(3);

            if (_mqtt.IsConnected)
            {
                //var msg = JsonConvert.SerializeObject(sensor);
                var msg = JsonConvert.SerializeObject(new  { clientid = sensor.ClientId, display = sensor.DisplayName, value = sensor.Value, time = sensor.MeasureTime.Value.ToString("dd.MM HH:mm", new System.Globalization.CultureInfo("ru-RU")) });
                _mqtt.Publish("testingsensors", Encoding.UTF8.GetBytes(msg));
            }

            _logger.DebugFormat("Setting sensor {0} value to {1} successfully finished", sensorId, val);
        }

        public class SensorTO
        {
            [JsonProperty(PropertyName = "sensorId")]
            public string SensorId { get; set; }

            [JsonProperty(PropertyName = "val")]
            public string SensorValue { get; set; }
        }
    }
}
