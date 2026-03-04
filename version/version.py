

#################################

# Algoritmo Versionamento

# coletar a machine_version de acordo com o datetime desejado
# criar a lista ligada a partir de previous_id até que o mesmo seja vazio (sinalizando início das alterações)
# PARA cada versão:
    # coletar os dados (machine, disks...) da versão
    # coletar version com as alterações com mesmo ref_id
    # mesclar dados relacionando table, from, to e changes, criando o dado concreto
# após iterar sobre todas as versões da lista ligada, retornar o conjunto de dados

#################################

import sqlite3
conn = sqlite3.connect('database.db')
cursor = conn.cursor()

COLUMNS_BY_TABLE = {
    'machine': ['mac', 'os', 'title', 'osInstallDate'],
    'disk': ['name', 'serialNumber', 'model'],
}

def generate_version(datetime:str):
    # coletar a machine_version de acordo com o datetime desejado
    cursor.execute('SELECT id, previous_id FROM machine_version WHERE datetime=?', (datetime, ))
    r = cursor.fetchone()

    if not r:
        print('nenhuma versão encontrada com o datetime informado')
        return
    
    # criar a lista ligada a partir de previous_id até que o mesmo seja vazio (sinalizando início das alterações)
    mversion_id, previous_id = r
    linked_list_versions = [mversion_id] # end to init

    while previous_id: # coletando previous_id
        linked_list_versions.append(previous_id)
        cursor.execute('SELECT previous_id FROM machine_version WHERE id=?', (previous_id, ))
        previous_id = cursor.fetchone()[0]

    # revertendo ordem para init to end
    linked_list_versions.reverse()
    # print('Versões:', linked_list_versions)

    # PARA cada versão:
    data = {}
    change_versions = {}

    for v in linked_list_versions:

        # coletar os dados (machine, disks...) da versão
        for table in COLUMNS_BY_TABLE:
            if table not in data:
                data[table] = {}
            
            cursor.execute(f'SELECT id, {','.join(COLUMNS_BY_TABLE[table])} FROM {table} WHERE machine_version_id=?', (v, ))
            data[table].update({ vals[0]: list(vals[1:]) for vals in cursor.fetchall() })

        # coletar version com as alterações com mesmo ref_id
        cursor.execute('SELECT `table`, from_id, to_id, changes FROM version WHERE ref_id=?', (v, ))
        r = cursor.fetchall()
        if not r:
            continue

        for l in r:
            if l[0] not in change_versions:
                change_versions[l[0]] = []

            change_versions[l[0]].append(l[1:])

    # print("Data:", data)
    # print("Changes:", change_versions)

    for table in data:
        for from_id, to_id, changes in change_versions[table]:
            old_val = data[table].pop(from_id)

            # caso seja remoção
            if to_id is None:
                print(f'<Change Table="{table}" Column="{change}" PreviousId="{from_id}" NewId=None>')
                continue

            new_val = data[table].pop(to_id)

            # mesclar dados relacionando table, from, to e changes, criando o dado concreto
            for change in changes.split('|'):
                index = COLUMNS_BY_TABLE[table].index(change)
                print(f'<Change Table="{table}" Column="{change}" PreviousId="{from_id}" NewId="{to_id}" OldValue="{old_val[index]}" NewValue="{new_val[index]}">')
                old_val[index] = new_val[index]
            
            data[table][to_id] = old_val # pega o novo id e insere com os campos atualizados


    print("Version from", datetime, data)

generate_version('2026-03-05 10:10:00')
cursor.close()