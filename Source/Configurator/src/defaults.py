
"""

HOC Configurator

Copyright (C) Souldbminer

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""


class Defaults: # This almost always never needs to be updated as pulling from the kip takes priority
    def __init__(self):
        self.autosave = 0
        pass
d = Defaults()