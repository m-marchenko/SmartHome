using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Net;

namespace SmartHome.Test
{
    [TestClass]
    public class ControllerCommandTest
    {
        [TestMethod]
        public void SetSensorValue_Test()
        {
            using (var client = new WebClient())
            {
                client.Headers.Add("content-type", "application/json");

                var data = "{\"sensorId\":\"101\", \"val\":\"24\"}";

                string response = client.UploadString("http://localhost/SmartHome/Command/SetSensorValue", data);

            

                
            }

        }
    }
}
