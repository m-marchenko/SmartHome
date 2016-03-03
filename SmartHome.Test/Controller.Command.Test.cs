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
                //string response = client.UploadString("http://smart.no-troubles.com/Command/SetSensorValue", data);




            }

        }

        [TestMethod]
        public void RecieveCommand_Test()
        {
            using (var client = new WebClient())
            {
                client.Headers.Add("content-type", "application/json");

                //var data = "{\"target\":\"60\"}";

                string response = client.DownloadString("http://localhost/SmartHome/Command/RecieveCommand?target=60");
                //string response = client.DownloadString("http://smart.no-troubles.com/Command/RecieveCommand?target=60");




            }

        }

    }
}
