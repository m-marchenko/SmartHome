using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Serialization;

namespace SmartHome.Models.DataContracts
{
    #region base classes

    public abstract class GenericObjectBase
    {
        protected GenericObjectBase()
        {

        }
        protected GenericObjectBase(string id, string name, string displayName)
        {
            Id = id;
            Name = name;
            DisplayName = displayName;
        }

        [XmlAttribute]
        public string DisplayName
        {
            get; set;
        }

        [XmlAttribute]
        public string Id
        {
            get; set;
        }

        [XmlAttribute]
        public string Name
        {
            get; set;
        }

        [JsonIgnore]
        [XmlIgnore]
        public CompositeObject Parent { get; set; }

        [XmlIgnore]
        public string ClientId
        {
            get
            {
                var result = Id;
                var p = Parent;
                while (p != null)
                {
                    result = String.Format("{0}_{1}", p.Id, result);
                    p = p.Parent;
                }

                return result;
            }
        }

    }

    [Serializable]
    public class CompositeObject : GenericObjectBase
    {
        //protected readonly ICollection<IGenericObject> _all = new List<IGenericObject>();

        protected CompositeObject() : base()
        {
        }

        protected CompositeObject(string id, string name, string displayName) : base(id, name, displayName)
        {
        }

        public List<CompositeObject> Units { get; set; }

        public List<SensorBase> Sensors { get; set; }

        public List<Command> Commands { get; set; }

        public void UpdateParents()
        {
            if (Units == null)
                return;

            foreach (var unit in this.Units)
            {
                unit.Parent = this;
                unit.UpdateParents();
            }

            foreach (var sensor in this.Sensors)
            {
                sensor.Parent = this;
            }

            foreach (var command in this.Commands)
            {
                command.Parent = this;
            }
        }

        public SensorBase FindSensor(string sensorId)
        {
            if (Sensors == null)
                return null;

            var result = Sensors.Where(s => s.HardId == sensorId).FirstOrDefault();
            if (result == null)
            {
                foreach(var unit in Units)
                {
                    result = unit.FindSensor(sensorId);

                    if (result != null)
                        break;
                }
            }

            return result;
        }

        public CompositeObject FindUnit(string unitId)
        {
            if (Units == null)
                return null;

            var result = Units.Where(s => s.ClientId == unitId).FirstOrDefault();
            if (result == null)
            {
                foreach (var unit in Units)
                {
                    result = unit.FindUnit(unitId);

                    if (result != null)
                        break;
                }
            }

            return result;

        }
    }

    public abstract class SensorBase : GenericObjectBase
    {
        public SensorBase()
        {
            // месяц при 5-минутном интервале
            History = new Queue<SensorHistoryEntry>(12*24*31);
        }

        /// <summary>
        /// это "железный" id. уникальный в рамках всего проекта
        /// </summary>
        [XmlAttribute]
        public string HardId { get; set; }
        public string MeasureUnit { get;  set; }
        public SensorType SensorType { get;  set; }
        public string Value { get; set; }

        public DateTime? MeasureTime { get; set; }

        [XmlIgnore]
        public Queue<SensorHistoryEntry> History { get; protected set; }
    }

    public class SensorHistoryEntry
    {
        public string Value { get; set; }

        public DateTime MeasureTime { get; set; }
    }


    #endregion

    public enum SensorType
    {
        Temperature,
        Pressure,
        Moisture,
        Level,
        State
    }

    #region Implementations

    [Serializable]
    public class TemperatureSensor : SensorBase
    {
        public TemperatureSensor()
        {
            SensorType = SensorType.Temperature;
            MeasureUnit = "град";
        }
    }

    public class PressureSensor : SensorBase
    {
        public PressureSensor()
        {
            SensorType = SensorType.Pressure;
            MeasureUnit = "мм.рт.ст";
        }
    }

    public class LevelSensor : SensorBase
    {
        public LevelSensor()
        {
            SensorType = SensorType.Level;
            MeasureUnit = "%";
        }
    }

    public class StateSensor : SensorBase
    {
        public StateSensor()
        {
            SensorType = SensorType.State;
        }
    }

    //[XmlInclude(typeof(House))]
    //[XmlInclude(typeof(Garden))]
    //[XmlInclude(typeof(Greenhouse))]
    //[XmlInclude(typeof(Barrel))]
    [XmlInclude(typeof(Command))]
    [XmlInclude(typeof(TemperatureSensor))]
    [XmlInclude(typeof(PressureSensor))]
    [XmlInclude(typeof(LevelSensor))]
    [XmlInclude(typeof(StateSensor))]
    public class RootUnit : CompositeObject
    {

    }

    //public class House : CompositeObject
    //{

    //}

    //public class Garden : CompositeObject
    //{

    //}


    //public class Greenhouse : CompositeObject
    //{

    //}

    //public class Barrel : CompositeObject
    //{

    //}
    public class Command : GenericObjectBase
    {

    }

    #endregion
}
