using SmartHome.Models;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
//using System.Web.Http;
using System.Web.Mvc;

namespace SmartHome.Controllers
{
    public class CommandController : Controller
    {
        private IRootObject _root;

        private static ConcurrentDictionary<string, string> _commands = new ConcurrentDictionary<string, string>();

        public CommandController(IRootObject root)
        {
            _root = root;
        }

        [HttpPost]        
        public void SendCommand(string command, string target)
        {
            _commands.AddOrUpdate(target, command, (k, v) => command);
        }

        [HttpGet]
        public string RecieveCommand(string target)
        {
            string result = null;

            _commands.TryRemove(target, out result);

            return result;
        }

        [HttpPost]
        public void SetSensorValue(string sensorId, string val)
        {
            var sensor = _root.FindSensor(sensorId);
            sensor.Value = val;
            sensor.MeasureTime = DateTime.Now;
        
        }
    }
}
