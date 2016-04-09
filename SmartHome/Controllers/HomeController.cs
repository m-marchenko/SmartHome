using SmartHome.Models;
using SmartHome.Models.DataContracts;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Web;
using System.Web.Mvc;
using System.Xml.Serialization;

namespace SmartHome.Controllers
{
    public class HomeController : Controller
    {
        //private IRootObject _root;

        private static RootUnit _root;

        public HomeController(RootUnit root)
        {
              _root = root;

            //if (_root != null) return;

            //_root = (RootUnit)(ConfigurationManager.GetSection("RootUnit"));
            //_root.UpdateParents();

        }


        public ActionResult MainView()
        {
            return View(_root);
        }

        //public ActionResult Refresh(string id)
        //{
        //    var m = _root.FindCompositeObject(id);

        //    return PartialView("CompositeObject", m);
        //}

        public ActionResult Index()
        {
            return View();
        }

        public ActionResult About()
        {
            ViewBag.Message = "Your application description page.";

            return View();
        }

        public ActionResult Contact()
        {
            ViewBag.Message = "Your contact page.";

            return View();
        }



        #region tmp

        private void CreateRootUnit()
        {
            _root = new RootUnit()
            {
                Id = "root",
                Name = "fazenda",
                DisplayName = "Фазенда",
                Sensors = new List<SensorBase>()
                {
                    new TemperatureSensor() { Id = "airtemp", Name = "temperature", DisplayName = "температура воздуха", HardId = "TM0001" },
                    new PressureSensor() { Id = "pressure", Name = "pressure", DisplayName = "атмосферное давление", HardId = "PR0001" },
                },
                Units = new List<CompositeObjectBase>()
                {
                    new House() {
                        Id = "house",
                        Name = "house",
                        DisplayName = "дом",
                        Sensors = new List<SensorBase>() {
                            new TemperatureSensor() { Id = "firstfloor", Name = "firstfloor", DisplayName = "температура на 1 этаже", HardId = "TM0002" },
                            new TemperatureSensor() { Id = "secondfloor", Name = "secondfloor", DisplayName = "температура на 2 этаже", HardId = "TM0003" },
                        },
                    },
                    new Greenhouse()
                    {
                        Id = "parnik1",
                        Name = "parnik1",
                        DisplayName = "теплица 1",
                        Sensors = new List<SensorBase>()
                        {
                            new TemperatureSensor() { Id = "temperature", Name = "temperature", DisplayName = "температура воздуха", HardId = "TM0004" }
                        },
                        Units = new List<CompositeObjectBase>()
                        {
                            new Barrel()
                            {
                                Id = "barrel1",
                                Name = "barrel1",
                                DisplayName = "бочка 1",
                                Commands = new List<Command>()
                                {
                                    new Command() { Id = "open", Name = "open", DisplayName = "открыть" },
                                    new Command() { Id = "close", Name = "close", DisplayName = "закрыть" }
                                },
                                Sensors = new List<SensorBase>()
                                {
                                    new LevelSensor() { Id = "level", Name = "level", DisplayName = "уровень", HardId = "LV0001" }
                                }
                            },

                            new Barrel()
                            {
                                Id = "barrel2",
                                Name = "barrel2",
                                DisplayName = "бочка 2",
                                Commands = new List<Command>()
                                {
                                    new Command() { Id = "open", Name = "open", DisplayName = "открыть" },
                                    new Command() { Id = "close", Name = "close", DisplayName = "закрыть" }
                                },
                                Sensors = new List<SensorBase>()
                                {
                                    new LevelSensor() { Id = "level", Name = "level", DisplayName = "уровень", HardId = "LV0002" }
                                }
                            }

                        }
                    }
                }

            };

            XmlSerializer xs = new XmlSerializer(_root.GetType());

            StringWriter sw = new StringWriter();

            xs.Serialize(sw, _root);

            var s = sw.ToString();

            var sr = new StringReader(s);

            var f = (RootUnit)xs.Deserialize(sr);

            var ss = "ok";

        }

        #endregion
    }
}