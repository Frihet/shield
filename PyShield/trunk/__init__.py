import DCOracle2, popen2

apilevel = '2.0'
threadsafety = 1
paramstyle = 'named'


class Connection(DCOracle2.connection):
    def __init__(self, dsn=None, user=None, password=None, database=None):
        dsnUser = dsnPassword = dsnDatabase = None
        if dsn:
            if '@' in dsn:
                dsn, dsnDatabase = dsn.split('@')
            if '/' in dsn:
                dsn, dsnPassword = dsn.split('/')
            dsnUser = dsn
        user = user or dsnUser
        password = password or dsnPassword
        database = database or dsnDatabase
            
        DCOracle2.connection.__init__(self, DCOracle2.dco2.connect(user, password, database))
        self.__dict__['shield_out'], self.__dict__['shield_in'] = popen2.popen2(
            'shield --user %(user)s --password %(password)s --host %(database)s 2> /dev/null' % {
            'user': user, 'password': password, 'database': database})
        
    def cursor(self):
        if self._db is None:
            raise InterfaceError, "The database connection is closed"
        c = self._db.cursor()
        return Cursor(c, self)

    def shield(self, query):
        self.shield_in.write(query)
        self.shield_in.write('\0')
        self.shield_in.flush()
        res = ['']
        while True:
            char = self.shield_out.read(1)
            if char == '\0':
                if res[-1] == '': break
                res.append('')
            else:
                res[-1] += char
        del res[-1] # Remove the empty last "line"
        return res

class Cursor(DCOracle2.cursor):
    def execute(self, operation=None, **kw):
        if operation is None:
            operations = [None]
        else:
            operations = self._connection.shield(operation)
        for operation in operations:
            DCOracle2.cursor.execute(self, operation, **kw)

    def executemany(self, operation, seq_of_parameters):
        operations = self._connection.shield(operation)
        for params in seq_of_parameters:
            for operation in operations:
                DCOracle2.cursor.execute(self, operation, **params)

        
