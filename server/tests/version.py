import flask, configparser
import datetime as dt
from models.machine import *
from models.user import UserModel, ADMIN_RULES
from server import Server

# app = flask.Flask(__name__)
# cfg = configparser.ConfigParser()
# with open('server.cfg') as f:
#     cfg.read_file(f)

# app.json.sort_keys = False
# app.json.ensure_ascii = False

# app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + 'tests_version.db'
# app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
# app.config['SECRET_KEY'] = cfg['flask']['secret_key']

# db.init_app(app)

server = Server(setup=True)
app = server.getApp()

with app.app_context():
    db.create_all()

    user = UserModel.query.filter_by(cpf='06658659109').first()
    if not user:
        user = UserModel(id=str(uuid4()), cpf='06658659109', name='Iago Carvalho')
        user.update_rules(ADMIN_RULES)
        user.set_password('1234')
        db.session.add(user)

    data = MachineVersionData(
        version=MachineVersionModel(mac='D8:5E:D3:E8:B6:ED', owner_id=user.id),
        machine=MachineModel(
            os='Microsoft Windows 11 Pro',
            title='GUI',
            os_architecture="64 bits",
            os_install_date=dt.datetime.strptime("2026-01-19 15:15:23", "%Y-%m-%d %H:%M:%S"),
            os_version="10.0.26200",
            os_serial_number="00330-80000-00000-AA779",
            organization=None,
            motherboard="B450M DS3H V2",
            motherboard_manufacturer="Gigabyte Technology Co., Ltd.",
            processor="AMD Ryzen 5 4500 6-Core Processor",
            processor_clock_speed="3.60 GHz"
        ),
        disks=[
            DiskModel(name="KINGSTON SNV2S1000G", serial_number=None, size='1000 GB', model="KINGSTON SNV2S1000G"),
        ],
        adapters=[
            NetworkAdapterModel(name="Realtek Gaming GbE Family Controller", mac="D8:5E:D3:E8:B6:ED"),
            NetworkAdapterModel(name="WAN Miniport (IP)", mac="04:76:20:52:41:53"),
        ],

        memories=[
            PhysicalMemoryModel(name="Physical Memory 0", capacity="8 GB", speed="2133 MT/s"),
            PhysicalMemoryModel(name="Physical Memory 2", capacity="8 GB", speed="2133 MT/s"),
        ],
        programs=[
            ProgramModel(
                name="Git", publisher="The Git Development Community", version="2.52.0",
                estimated_size="346 MB", current_user_only=False,
            ),
            ProgramModel(
                name="IntelliJ IDEA 2025.3.2", publisher="JetBrains s.r.o.", version="253.30387.90",
                estimated_size="0 MB", current_user_only=False,
            ),
            ProgramModel(
                name="Riot Vanguard", publisher="Riot Games, Inc.", version="2.52.0",
                estimated_size="0 MB",
                current_user_only=False,
            ),
            ProgramModel(
                name="Valorant", publisher="Riot Games, Inc.", version="2.55.0",
                estimated_size="28 GB",
                current_user_only=True,
            ),
        ]
    )

    MachineVersionModel.new(data, 'str')
    # print(*MachineVersionModel.get('D8:5E:D3:E8:B6:ED', 'mac', user.id, True, {'machine_version_id'}).items(), sep='\n')
