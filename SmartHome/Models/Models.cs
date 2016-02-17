using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SmartHome.Models
{

    #region Interfaces

    public interface IRootObject : ICompositeObject
    {
        ISensor FindSensor(string sensorId);
    }

    public interface IGenericObject
    {
        string Id { get; }

        string Name { get; }

        string DisplayName { get; }

    }

    public interface ICompositeObject : IGenericObject
    {
        ICollection<ISensor> Sensors { get; }

        ICollection<IGenericObject> All { get; }

        ICollection<IControlUnit> ControlUnits { get; }

        ICollection<ICompositeObject> GetCompositeObjects(bool includeConrolUnits = true);

        ICompositeObject AddSensor(ISensor sensor);

        ICompositeObject AddCompositeObject(ICompositeObject cobject);

        ICompositeObject AddControlUnit(IControlUnit unit);
    }

    public interface ISensor : IGenericObject
    {
        SensorType SensorType { get; }

        string MeasureUnit { get; }

        string Value { get; set; }

        DateTime MeasureTime { get; set; }
    }


    public interface IControlUnit : ICompositeObject
    {
        List<ICommand> AvailableCommands { get; }

        Dictionary<string, string> Settings { get; }
        void ExecuteCommand(ICommand command);
    }


    public interface ICommand : IGenericObject
    {
        bool CanExecute { get; set; }
    }

    #endregion

    #region Enums

    public enum SensorType
    {
        Temperature,
        Pressure,
        Moisture,
        Level,
        State
    }

    #endregion

    #region Implementations

    public abstract class GenericObjectBase : IGenericObject
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
            get; protected set;
        }

        public string Id
        {
            get; protected set;
        }

        public string Name
        {
            get; protected set;
        }
    }

    public abstract class CompositeObjectBase : GenericObjectBase, ICompositeObject
    {
        protected readonly ICollection<IGenericObject> _all = new List<IGenericObject>();

        protected CompositeObjectBase() : base()
        {
        }

        protected CompositeObjectBase(string id, string name, string displayName) : base(id, name, displayName)
        {
        }

        public ICollection<IGenericObject> All { get { return _all; } }
        public ICollection<ISensor> Sensors { get { return _all.OfType<ISensor>().ToList(); } }

        public ICollection<IControlUnit> ControlUnits
        {
            get
            {
                return _all.OfType<IControlUnit>().ToList();
            }
        }

        protected void AddSensorInternal(ISensor sensor)
        {
            _all.Add(sensor);
        }

        protected void AddCompositeObjectInternal(ICompositeObject obj)
        {
            _all.Add(obj);
        }

        protected void AddContolUnitInternal(IControlUnit unit)
        {
            _all.Add(unit);
        }

        public ICollection<ICompositeObject> GetCompositeObjects(bool includeConrolUnits = true)
        {
            var query = _all.OfType<ICompositeObject>();

            if (!includeConrolUnits)
                query = query.Where(c => !(c is IControlUnit));

            return query.ToList();
        }

        public ICompositeObject AddSensor(ISensor sensor)
        {
            AddSensorInternal(sensor);
            return this;
        }

        public ICompositeObject AddCompositeObject(ICompositeObject cobject)
        {
            AddCompositeObjectInternal(cobject);
            return this;
        }

        public ICompositeObject AddControlUnit(IControlUnit unit)
        {
            AddContolUnitInternal(unit);
            return this;
        }
    }

    public class Fazenda : CompositeObjectBase, IRootObject
    {
        public Fazenda()
            : base()
        {
            Id = "0";
            Name = "Fazenda";
            DisplayName = "Фазенда";

            #region Объекты недвижимости

            this
                // Дом
                .AddCompositeObject(new House()
                                        .AddSensor(new TemperatureSensor("100", "t3", "температура батареи") { Value = "63" })
                                        .AddSensor(new TemperatureSensor("101", "t4", "температура на 1 этаже"))
                                        .AddSensor(new TemperatureSensor("102", "t5", "температура на 2 этаже")))
                // Гараж
                .AddCompositeObject(new Garage("40", "gar1", "Гараж"))
                // Участок
                .AddCompositeObject(new Garden("51", "gdn1", "Участок")
                                        .AddSensor(new MovementSensor("103", "ms2", "датчик движения на гараже")))  // Датчик движения

                // Теплица 1
                .AddCompositeObject(new GreenHouse("53", "grh1", "Теплица 1")
                                        .AddSensor(new TemperatureSensor("104", "t104", "температура"))
                                        .AddSensor(new StateSensor("108", "oc108", "состояние двери") { Value = "закрыта" })
                                        .AddCompositeObject(new Barrel("60", "barrel60", "Бочка 1")
                                                                .AddSensor(new LevelSensor("105", "l105", "уровень")))
                                        .AddCompositeObject(new Barrel("63", "barrel63", "Бочка 2")
                                                                .AddSensor(new LevelSensor("106", "l106", "уровень")))

                                                                )

                // Теплица 2
                .AddCompositeObject(new GreenHouse("52", "grh2", "Теплица 2")
                                        .AddCompositeObject(new Barrel("61", "barrel61", "Бочка 1")
                                                                .AddSensor(new LevelSensor("106", "l106", "уровень")))
                                        .AddSensor(new TemperatureSensor("107", "t107", "температура"))
                                        .AddSensor(new StateSensor("109", "oc109", "состояние двери") { Value = "закрыта" })

                );

            #endregion

            #region Датчики


            #endregion
        }

        public ISensor FindSensor(string sensorId)
        {
            return FindSensor(sensorId, this);
        }

        private ISensor FindSensor(string id, ICompositeObject cobject)
        {
            var result = cobject.Sensors.Where(s => s.Id == id).FirstOrDefault();

            if (result == null)
            {
                foreach (var cobj in cobject.GetCompositeObjects())
                {
                    result = FindSensor(id, cobj);
                    if (result != null)
                        break;
                }
            }

            return result;
        }
    }

    public class House : CompositeObjectBase
    {
        public House()
        {
            Id = "1";
            Name = "House";
            DisplayName = "Дом";
        }
    }

    public class Garage : CompositeObjectBase
    {
        // ids starting from 40
        public Garage(string id, string name, string displayName)
            : base(id, name, displayName)
        {

        }
    }

    public class Garden : CompositeObjectBase
    {
        // ids starting from 30
        public Garden(string id, string name, string displayName)
            : base(id, name, displayName)
        {
        }

    }

    public class GreenHouse : CompositeObjectBase, IControlUnit
    {
        private List<ICommand> _commands = new List<ICommand>() {
            new GenericCommand() { Id = "1",  Name = "OpenDoor", DisplayName = "Открыть дверь" },
            new GenericCommand() { Id = "2", Name = "CloseDoor", DisplayName = "Закрыть дверь" }
        };

        // ids starting from 50
        public GreenHouse(string id, string name, string displayName)
            : base(id, name, displayName)
        {
        }

        public List<ICommand> AvailableCommands
        {
            get
            {
                return _commands;
            }
        }

        public Dictionary<string, string> Settings
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public void ExecuteCommand(ICommand command)
        {
            throw new NotImplementedException();
        }
    }

    public class Barrel : CompositeObjectBase, IControlUnit
    {

        private List<ICommand> _commands = new List<ICommand>() {
            new GenericCommand() { Id = "3",  Name = "Fill", DisplayName = "Залить" },
            new GenericCommand() { Id = "4", Name = "Empty", DisplayName = "Слить" }
        };

        public Barrel(string id, string name, string displayName)
            : base(id, name, displayName)
        {
        }

        public List<ICommand> AvailableCommands
        {
            get
            {
                return _commands;
            }
        }

        public Dictionary<string, string> Settings
        {
            get
            {
                throw new NotImplementedException();
            }
        }

        public void ExecuteCommand(ICommand command)
        {
            throw new NotImplementedException();
        }

    }

    public class GenericCommand : ICommand
    {
        public bool CanExecute
        {
            get; set;
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
    }

    #region Sensors

    public abstract class SensorBase : GenericObjectBase, ISensor
    {
        protected SensorBase()
        {
            
        }

        protected SensorBase(string id, string name, string displayName)             
            : base(id, name, displayName)
        {
            MeasureTime = DateTime.Now;
        }

        public string MeasureUnit { get; protected set; }
        public SensorType SensorType { get; protected set; }
        public string Value { get; set; }

        public DateTime MeasureTime { get; set; }
    }

    public class TemperatureSensor : SensorBase
    {
        private TemperatureSensor()
        { }

        public TemperatureSensor(string id, string name, string displayName) : base(id, name, displayName)
        {

            SensorType = SensorType.Temperature;
            MeasureUnit = "град";
        }

    }

    public class MovementSensor : SensorBase
    {
        public MovementSensor(string id, string name, string displayName) : base(id, name, displayName)
        {

            SensorType = SensorType.State;
            MeasureUnit = "";
        }
    }

    public class LevelSensor : SensorBase
    {
        public LevelSensor(string id, string name, string displayName) : base(id, name, displayName)
        {

            SensorType = SensorType.Level;
            MeasureUnit = "%";
        }

    }

    /// <summary>
    /// Датчик состояния
    /// </summary>
    public class StateSensor : SensorBase
    {
        public StateSensor(string id, string name, string displayName) : base(id, name, displayName)
        {

            SensorType = SensorType.State;
            MeasureUnit = "";
        }

    }

    #endregion

    #endregion

}
