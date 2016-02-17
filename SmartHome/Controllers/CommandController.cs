using SmartHome.Models;
using System;
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

        public CommandController(IRootObject root)
        {
            _root = root;
        }

        [HttpPost]        
        public void SendCommand(string command, string target)
        {

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
