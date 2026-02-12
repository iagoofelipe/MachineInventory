-- database: ../database.db

-- DELETE FROM machine;
-- DELETE FROM user;

INSERT INTO user (id, name, cpf) VALUES
    ('3b425bb05-c1a5d71484582211', 'Iago Carvalho', '123.456.789-10'),
    ('32e6bcda66-1919ab2b53fda09f', 'José Carvalho', '321.654.987-14');

INSERT INTO machine
    (id, title, mac, owner_id) VALUES
    ('3813a8513-dfc1136fac3700b1', 'Máquina Pessoal', 'D0:94:66:DC:BB:5Q', '3b425bb05-c1a5d71484582211'),
    ('3813a8aab-8aaa98666fed837', 'Máquina Pessoal', 'D0:94:66:DC:BB:4D', '32e6bcda66-1919ab2b53fda09f'),
    ('3813a8ab1-a7982371c415766a', 'Máquina Trabalho', 'D0:94:66:DC:BB:2E', '3b425bb05-c1a5d71484582211');

INSERT INTO machine_extra
    (id, config_date, os, os_architecture, os_install_date, os_version, os_serial_number, organization, motherboard_manufacturer, processor, machine_id) VALUES
    ('3a0b08d4a-ba3f071a9a391250', '2026-02-12 14:00:00', 'Microsoft Windows 11 Pro', '64 bits', '2025-10-22 19:31:11', '10.0.26100', '00380-51207-50199-SSQER', 'Trabalho ABC', 'Dell Inc.', 'Intel(R) Core(TM) i5-8500T CPU @ 2.10GHz', '3813a8ab1-a7982371c415766a'),
    ('2f80cec3a-2aa2922b5c8ca3e7', '2026-02-12 14:35:00', 'Microsoft Windows 11 Pro', '64 bits', '2025-10-22 19:31:11', '10.0.26100', '00380-51207-50199-SSQER', 'Trabalho ABC', 'Dell Inc.', 'Intel(R) Core(TM) i5-8500T CPU @ 2.10GHz', '3813a8ab1-a7982371c415766a'),
    ('2f8578260-6d00005eba70b0b4', '2026-02-12 10:00:00', 'Microsoft Windows 11 Pro', '64 bits', '2025-07-12 10:35:00', '10.0.26100', '11387-51368-54488-SABRC', NULL, 'Gigabyte', 'Intel(R) Core(TM) i7 CPU @ 2.4GHz', '3813a8513-dfc1136fac3700b1'),
    ('2fed3c5e3-2245bb7222c7de6d', '2026-02-12 12:40:00', 'Microsoft Windows 11 Pro', '64 bits', '2026-02-12 10:35:00', '10.0.26100', '11387-51368-54488-JFOYB', NULL, 'Gigabyte', 'Intel(R) Core(TM) i7 CPU @ 2.4GHz', '3813a8aab-8aaa98666fed837');

-- INSERT INTO machine_disk (id, name, serialNumber, size, model, machineId) VALUES
--     ('3c9387df4-24bb51b06ac74f2d', 'NVMe KINGSTON SNV2S50', '0000_0000_0000_0000_0026_B768_63EB_ECB5.', 500105249280, 'NVMe KINGSTON SNV2S50','3a0b08d4a-ba3f071a9a391250');

-- INSERT INTO machine_network_adapter (id, name, mac, machineId) VALUES 
--     ('3d0980e67-c404e4eb89575826', 'Intel(R) Ethernet Connection (7) I219-LM', 'D0:94:66:DC:BB:2E', '3a0b08d4a-ba3f071a9a391250'),
--     ('3d0980e84-349a14210f5a8f9f', 'WAN Miniport (IP)', '08:BB:20:52:41:53', '3a0b08d4a-ba3f071a9a391250'),
--     ('3d0980e8a-1b41cb2f83b9d015', 'WAN Miniport (IPv6)', '0A:B9:20:52:41:53', '3a0b08d4a-ba3f071a9a391250'),
--     ('3d0980e8f-193b27d5e625112a', 'WAN Miniport (Network Monitor)', '0C:53:20:52:41:53', '3a0b08d4a-ba3f071a9a391250');

-- INSERT INTO machine_physical_memory (id, name, capacity, speed, machineId) VALUES
--     ('3e2e24931-2721e049a22d16ec', 'Physical Memory 0', '4294967296', '2666', '3a0b08d4a-ba3f071a9a391250'),
--     ('3e2e24944-6d76ddba28399d4b', 'Physical Memory 1', '4294967296', '2666', '3a0b08d4a-ba3f071a9a391250');


-- INSERT INTO program (id, name, version, publisher, estimatedSize, currentUserOnly) VALUES
--     ('3eae9570b-8a7e98889f31b915', '7-Zip 22.01 (x64)', '22.01', '', 5601, 0),
--     ('3eae95727-334fc715c09867c3', 'Support Button 24.3.3 [suporteremoto.serpro.gov.br]', '24.3.3', '', 0, 0),
--     ('3eae95749-f1579d1c3f9ba3da', 'Mozilla Maintenance Service', '78.9.0', '', 308, 0),
--     ('3eae95754-8873910ce25a1c73', 'OCS Inventory NG Agent 2.10.1.0-01', '2.10.1.0-01', '', 0, 0),
--     ('3eae95757-d0b64cd64b450f02', 'Microsoft Office Professional Plus 2013', '15.0.4569.1506', '', 0, 0),
--     ('3eae9575f-5214e4fa96b132a', 'Support Button 25.3.1 [suporteremoto.serpro.gov.br]', '25.3.1', '', 0, 0),
--     ('3eae95766-8ae99ee2d69463c0', 'Python 3.13.9 Executables (64-bit)', '3.13.9150.0', '', 2616, 0),
--     ('3eae95769-a09923d5f4136f63', 'Python 3.13.9 Test Suite (64-bit)', '3.13.9150.0', '', 33316, 0),
--     ('3eae9576d-976ca2adab1b4be3', 'Python 3.13.9 pip Bootstrap (64-bit)', '3.13.9150.0', '', 288, 0),
--     ('3eae95770-2935b2d337edbd12', 'Python 3.13.9 Core Interpreter (64-bit)', '3.13.9150.0', '', 6040, 0),
--     ('3eae95773-d1c36515b9ab8f3c', 'Python 3.13.9 Documentation (64-bit)', '3.13.9150.0', '', 60660, 0),
--     ('3eae95776-47a140e049a3dc35', 'Python 3.13.9 Add to Path (64-bit)', '3.13.9150.0', '', 52, 0),
--     ('3eae95779-5460bbe14ea50add', 'Python 3.13.9 Tcl/Tk Support (64-bit)', '3.13.9150.0', '', 13872, 0),
--     ('3eae9577c-ddf4d08c29755ddd', 'Python 3.13.9 Standard Library (64-bit)', '3.13.9150.0', '', 23428, 0),
--     ('3eae9577f-6121c17295a77abc', 'Microsoft Office Professional Plus 2013', '15.0.4569.1506', '', 2149554, 0),
--     ('3eae95782-15e3172e8f6fcc90', 'Microsoft Access MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 122314, 0),
--     ('3eae95786-466b7bff052990cd', 'Microsoft Excel MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 92878, 0),
--     ('3eae95789-c91a1d36f4de8977', 'Microsoft PowerPoint MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 31092, 0),
--     ('3eae9578c-b31ac87fd9801a41', 'Microsoft Publisher MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 16975, 0),
--     ('3eae9578f-635a42116dad4ef6', 'Microsoft Outlook MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 310559, 0),
--     ('3eae95792-fb7f7073824e37cb', 'Microsoft Word MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 17976, 0),
--     ('3eae95795-82fe8ca0b4d48ea', 'Microsoft Office Proofing Tools 2013 - English', '15.0.4569.1506', '', 22476, 0),
--     ('3eae95798-273f4c4c6ce7b657', 'Revisores de Texto do Microsoft Office 2013 – Português do Brasil', '15.0.4569.1506', '', 24052, 0),
--     ('3eae9579b-1f5640cf389cc16a', 'Microsoft Office Proofing Tools 2013 - Español', '15.0.4569.1506', '', 24139, 0),
--     ('3eae9579f-41fbd39e94a3ebac', 'Microsoft Office Proofing (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 1870, 0),
--     ('3eae957b7-6b604c4876ee5d12', 'Microsoft InfoPath MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 73140, 0),
--     ('3eae957bb-850c507e5ec8fd82', 'Microsoft Office Shared MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 120931, 0),
--     ('3eae957bf-a664076165b9fb8a', 'Microsoft DCF MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 38470, 0),
--     ('3eae957c2-4f0ec68e5b154695', 'Microsoft OneNote MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 91401, 0),
--     ('3eae957c5-14c76ca055bbc81e', 'Microsoft Groove MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 15272, 0),
--     ('3eae957c8-4ac2fcf625a6791a', 'Microsoft Office 32-bit Components 2013', '15.0.4569.1506', '', 234027, 0),
--     ('3eae957cb-32b9ca7ca529ec60', 'Microsoft Office Shared 32-bit MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 16953, 0),
--     ('3eae957cf-b7f6a23cdedc031e', 'Microsoft Office OSM MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 974, 0),
--     ('3eae957d2-4d1e4902dfcfff29', 'Microsoft Office OSM UX MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 8296, 0),
--     ('3eae957d6-f8f3ef189ed21dfd', 'Microsoft Lync MUI (Portuguese (Brazil)) 2013', '15.0.4569.1506', '', 5981, 0),
--     ('3eae957d9-aecfa2068fa5baea', 'GLPI Agent 1.15', '1.15', '', 98834, 0),
--     ('3eae957dc-e320ee20123efe21', 'Microsoft Update Health Tools', '5.72.0.0', '', 1045, 0),
--     ('3eae957df-c1d71f55a2fd66a4', 'Google Chrome', '', 'Google LLC', 135513, 0),
--     ('3eae957e3-9d3a57264dffe2db', 'Python 3.13.9 Development Libraries (64-bit)', '3.13.9150.0', '', 2832, 0),
--     ('3eae957e6-f1103e0ea35e70ad', 'CodeBlocks', '25.03', '', 0, 1),
--     ('3eae957e9-9ac8950833731684', 'Figma', '', '', 135933, 1),
--     ('3eae957ec-67ac46a6a689063b', 'Git', '', '', 353218, 1),
--     ('3eae957ef-1576190f8f73a585', 'Postman x64 11.81.0', '11.81.0', 'Postman', 142911, 1),
--     ('3eae957f2-4a9363e2bb4cb392', 'Qt Designer', '', '', 77556, 1),
--     ('3eae957f6-8dde93e3c2fb6aa8', 'Programa Tópico Editado', '', '', 25681, 1),
--     ('3eae957f9-70e5b692974cf303', 'Python 3.13.9 (64-bit)', '3.13.9150.0', '', 163898, 1),
--     ('3eae957fc-6de78ebcf1b66820', 'Microsoft Visual Studio Code (User)', '1.109.2', '', 494162, 1),
--     ('3eae957ff-2bcfa4d6ddc9b211', 'MSYS2', '', '', 332415, 1);

-- INSERT INTO machine_program (id, programId, machineId) VALUES
--     ('45ad6b5e8-ac69d38079374836', '3eae9570b-8a7e98889f31b915', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b622-cae2632b9374c064', '3eae95727-334fc715c09867c3', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b628-1c136ac4efaaa40b', '3eae95749-f1579d1c3f9ba3da', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b62d-fa5c4f4621365e82', '3eae95754-8873910ce25a1c73', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b631-b337462e351aff45', '3eae95757-d0b64cd64b450f02', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b655-160bf88f0dafb747', '3eae9575f-5214e4fa96b132a', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b65c-e0171157ab8daa98', '3eae95766-8ae99ee2d69463c0', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b660-2d79f2a3387aa383', '3eae95769-a09923d5f4136f63', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b664-ecc330da0c5cff0e', '3eae9576d-976ca2adab1b4be3', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b668-b125b8486abb9e0f', '3eae95770-2935b2d337edbd12', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b66c-c7ee1d65ce7d140f', '3eae95773-d1c36515b9ab8f3c', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b670-dcf1a04929510b08', '3eae95776-47a140e049a3dc35', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b674-ea622e74bf8356ea', '3eae95779-5460bbe14ea50add', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b678-bccfb7d565ecae70', '3eae9577c-ddf4d08c29755ddd', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b67c-c44812a5da91d513', '3eae9577f-6121c17295a77abc', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b67f-f919cb9796814aa8', '3eae95782-15e3172e8f6fcc90', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b683-963d15a10d7f5652', '3eae95786-466b7bff052990cd', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b687-886a2f8b1668d13', '3eae95789-c91a1d36f4de8977', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b68b-406c6b65a9aededa', '3eae9578c-b31ac87fd9801a41', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b68f-cd85c52f61a6615a', '3eae9578f-635a42116dad4ef6', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b695-c14b6739023fb75e', '3eae95792-fb7f7073824e37cb', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b699-ee9e018ab5bdd07c', '3eae95795-82fe8ca0b4d48ea', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b69c-e3d2451ff8f379e3', '3eae95798-273f4c4c6ce7b657', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6a0-e07291f8e898f6cf', '3eae9579b-1f5640cf389cc16a', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6a4-10d07960be1052bb', '3eae9579f-41fbd39e94a3ebac', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6a8-bc092736dd50943a', '3eae957b7-6b604c4876ee5d12', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6ac-6818c67c2691392d', '3eae957bb-850c507e5ec8fd82', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6b0-545797fb91dafd56', '3eae957bf-a664076165b9fb8a', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6b4-16890fbd31f9ef45', '3eae957c2-4f0ec68e5b154695', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6b8-1aa9ddc8116c58dd', '3eae957c5-14c76ca055bbc81e', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6bb-7f6d173cacceb30e', '3eae957c8-4ac2fcf625a6791a', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6bf-f3b1040958bd2d4a', '3eae957cb-32b9ca7ca529ec60', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6c3-6786a74ba009b383', '3eae957cf-b7f6a23cdedc031e', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6c7-e9c92c6b3db0051', '3eae957d2-4d1e4902dfcfff29', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6cb-92ffd29b176192b6', '3eae957d6-f8f3ef189ed21dfd', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6cf-86c5e4e06e2d649d', '3eae957d9-aecfa2068fa5baea', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6d3-1d38da81a358744e', '3eae957dc-e320ee20123efe21', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6d7-47ee7ba92de60fb8', '3eae957df-c1d71f55a2fd66a4', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6db-dd7f9a705f16b497', '3eae957e3-9d3a57264dffe2db', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6e1-687a32b54404550b', '3eae957e6-f1103e0ea35e70ad', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6e5-c0a3f5d4f349372a', '3eae957e9-9ac8950833731684', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6e8-135feb985acd7a38', '3eae957ec-67ac46a6a689063b', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6ec-b901fa1abddd0740', '3eae957ef-1576190f8f73a585', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6f0-bfc2b07cb39a5fd1', '3eae957f2-4a9363e2bb4cb392', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6f4-efd99a6d6cce6356', '3eae957f6-8dde93e3c2fb6aa8', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6f8-b4582ba7e9306615', '3eae957f9-70e5b692974cf303', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b6fc-b84e2b8197f0bd7', '3eae957fc-6de78ebcf1b66820', '3a0b08d4a-ba3f071a9a391250'),
--     ('45ad6b700-511a348081a6a044', '3eae957ff-2bcfa4d6ddc9b211', '3a0b08d4a-ba3f071a9a391250');


SELECT * FROM machine;
