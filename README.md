# SerLib
SerLib2 is the successor of our earlier SerLib library.

# Status
Type        | Status
----------- | -------
Linux & OSX | [![Build Status](https://travis-ci.org/Zefiros-Software/SerLib2.svg?branch=master)](https://travis-ci.org/Zefiros-Software/SerLib2)
Windows     | [![Build status](https://ci.appveyor.com/api/projects/status/2cdkl820n5ohn9i7?svg=true)](https://ci.appveyor.com/project/PaulVisscher/serlib2)
Coverage	| [![codecov](https://codecov.io/gh/Zefiros-Software/SerLib2/branch/master/graph/badge.svg)](https://codecov.io/gh/Zefiros-Software/SerLib2)

# [ZPM](http://zpm.zefiros.eu) Installation
In `.package.json`
```json
"requires": [
		{
			"name": "Zefiros-Software/SerLib2",
			"version": "@head"
		}
]
```

In `premake5.lua`
```lua
zpm.uses "Zefiros-Software/Serlib2"
```

## Bugs
When a bug is found, please insert it in the issue tracker, so we can resolve it as quickly as we can.

## Contributing
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request

## License
This project is licensed under the MIT license by [Zefiros Software](https://zefiros.eu).

```
Copyright (c) 2017 Zefiros Software.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```