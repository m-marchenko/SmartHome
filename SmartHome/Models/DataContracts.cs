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

        public string DisplayName
        {
            get; set;
        }

        public string Id
        {
            get; set;
        }

        public string Name
        {
            get; set;
        }

        [XmlIgnore]
        public CompositeObjectBase Parent { get; set; }

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
    public abstract class CompositeObjectBase : GenericObjectBase
    {
        //protected readonly ICollection<IGenericObject> _all = new List<IGenericObject>();

        protected CompositeObjectBase() : base()
        {
        }

        protected CompositeObjectBase(string id, string name, string displayName) : base(id, name, displayName)
        {
        }

        public List<CompositeObjectBase> Units { get; set; }

        public List<SensorBase> Sensors { get; set; }

        public List<Command> Commands { get; set; }

        public void UpdateParents()
        {
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
    }

    public abstract class SensorBase : GenericObjectBase
    {
        public SensorBase()
        {

        }

        /// <summary>
        /// это "железный" id. уникальный в рамках всего проекта
        /// </summary>
        public string HardId { get; set; }
        public string MeasureUnit { get;  set; }
        public SensorType SensorType { get;  set; }
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

    [XmlInclude(typeof(House))]
    [XmlInclude(typeof(Greenhouse))]
    [XmlInclude(typeof(Barrel))]
    [XmlInclude(typeof(Command))]
    [XmlInclude(typeof(TemperatureSensor))]
    [XmlInclude(typeof(PressureSensor))]
    [XmlInclude(typeof(LevelSensor))]
    public class RootUnit : CompositeObjectBase
    {

    }

    public class House : CompositeObjectBase
    {

    }

    
    public class Greenhouse : CompositeObjectBase
    {

    }

    public class Barrel : CompositeObjectBase
    {

    }
    public class Command : GenericObjectBase
    {

    }

    #endregion
}
