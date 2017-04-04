<h1>PwdManLib - Password Management Libraries</h1>

<h2>Problem:</h2>
</p>
    Security is an ever-increasingly complex field and taking into consideration all of the factors that go into securing
    user accounts during the software engineering lifecycle is an immense task. Many companies and developers find their
    zero-days / security-holes AFTER deployment of their systems, which exposes their users to to data-loss, identity-theft, and financial issues, which in turn, lowers the companies credibility and can cost the company and the their
    development, both time and money.
</p>

<h2>Solution:</h2>
<p>
    What if the development team was able to create their system without writing their own account management system?
    That is exactly the goal of PwdManLib. To make a set security tools that is tested and confirmed by the community.
    This project aims to implement a set of C/C++ libraries for developers to use in user / accnt password management.
    These tools are aimed at making user / account management easier for developers to implement in their projects. 
</p>

<h2>Requirements:</h2>
<ol>
    <li>The project should be open source and FREE for anyone to use.</li>
    <li>The libraries should be extensible and pluggable with multiple frameworks.</li>
    <li>Security  should be built-in and have an abstraction layer for developement use.</li> 
    <li>Should be Cross platform compatible, including mobile devices.</li>
    <li>Users should be able to store passwordsfrom multiple accounts in one place.</li>
    <li>Password hashes, salts, and accounts should be stored on remote / cloud DB.</li>
    <li>Must support ssl / https / and other encrypted networking protocols.</li>
    <li>Libraries should include support for authentication including multifactor auth.</li>
    <li>Multiple forms of encryption should be available, including:</li>
    <ul>
        <li>standard encryption algorithms: Triple DES, RSA, Blowfish, Twofish, AES</li>
    </ul>
    <li>Support for hashing passwords must be included, using:</li>
    <ul>
        <li>standard hashing algorithms: SHA256, SHA512, RipeMD, or WHIRLPOOL algorithms</li>
    </ul>
    <li>Support for hashing passwords with key-stretching algorithms:
    <ul>
        <li>standard key-streching hash-algorithms:  PBKDF2, bcrypt, scrypt, Argon2, PBKDF2
    </ul>
    <li>Support for captcha and account lockout handling.
    <li>Support for time-limited token-based automated password recovering, through email.
</ol>

<h3>Features:</h3>
<p>These are some of the proposed features that we would like to implement in the future, once requirements have been met.</p>

<ul>
    <li>Import From Browsers</li>
    <li>Import From Competitors</li>
    <li>Multi-Factor Authentication</li>
    <li>Export Data</li>
    <li>Automatic Password Capture</li>
    <li>Automatic Password Replay</li>
    <li>Fill Web Forms</li>
    <li>Multiple Form-Filling Identities</li>
    <li>Actionable Password Strength Report</li>
    <li>Browser Menu Of Logins</li>
    <li>Application Passwords</li>
    <li>Secure Sharing</li>
    <li>Digital Legacy</li>
</ul>

<h3>Alternatives:</h3>
<p>These are some of the alternative solutions that were considered / compared to solve this problem.</p>

<ul>
    <li>Flask-Login</li>
    <li>Flask-Security</li>        
    <li>PHPSEC Password Management</li>
    <li>Google Keyczar</li>
    <li>Java crypto.spec</li>
    <li>GuardianProject cacheword</li>
    <li>Master Password</li>
    <li>PHP Password Library</li>
</ul>

<h3>Project Timeline:</h3>
<p>
    The timeline as of writing, will end with the initial release 1.0.0A<br>
    This project will be started as an academic project and this timeline corresponds to a weekly schedule as such.<br>
    From the initial release onward, the timeline will reflect a schedule up-to the next release date.<br>
    Dates and release details will be determined by the community of committer's to the project.<br>
</p>

<hr>
<ul>
    <p><em>Week1</em>:<br>Finish documentation for project including a contributing.md page and define initial project structure / architecture.
                      Define api structure (if needed) and usage of libraries in detail.</p>
    <p><em>Week2</em>:<br>Define dependencies and framework / platforms that will be supported and how.
                      Start construction of hashing and encryption utilities (using libraries for the crypto).</p>
    <p><em>Week3</em>:<br>Test and debug hashing / encryption utils. Define interfaces for framework communication and adapters as needed. 
                      Define network architecture and start construction of interfaces.</p>
    <p><em>Week4</em>:<br>Construction of network interfaces continues and plugins to initially supported frameworks. 
                      Define authentication methods for initial release and start construction of auth libraries.</p>
    <p><em>Week5</em>:<br>Test and debug network libraries, auth libraries, and interface libraries. Thorough vetting of auth process occurs. 
                      Define methods and routines needed for password recovery and captcha verification.</p>
    <p><em>Week6</em>:<br>Finish / close out any outstanding issues and ensure all major bugs are fixed. Test, debug, and confirm all functionalities
                      needed for initial release / requirements are met. Code cleanup and documentation additions as needed. Minor bug fixes if time permits.</p>
</ul>
<hr>

<h4>Contributing:</h4>
<p>Guidelines for contributing to the project can be found within this repo, in CONTRIBUTING.md</p>

<h4>Contact:</h4>
<p>To contact the project director for more information or to provide feedback, please see the following links:</p>

<p style="text-indent: 20px;">Website:
    <a href="https://devopsec.net" style="margin-left: 80px;" target="_blank">dev<span style="display: none;">deob</span>ops<span style="display: none;">fusc</span>ec.n<span style="display: none;">ation</span>et</a>
</p>
<p style="text-indent: 20px;">Github:
    <a href="https://github.com/devopsec" style="margin-left: 90px;" target="_blank">gith<span style="display: none;">deob</span>ub.co<span style="display: none;">fusc</span>m/dev<span style="display: none;">ation</span>opsec</a>
</p>

<h3>Updates:</h3>
<p>
    Project is kicking off! Expect more updates to documentation, adding dependencies to tree, some architecture definitions and preliminary structures / wrappers.<br>
    Also, note that the project will be licensed under Apache License V2 now instead of GPL  to be more openly consumable and flexible for anyone to use the libraries!<br>
    Many changes coming upstream, the underlying architecture has been laid out, although much of it needs wrappers still.<br>
    The server is close to complete and database is well underway as well. Many neat oop functions nested in there too.<br>
    Biicode as a dependency is being deprecated as well and we are going with bash scripts for dependencies and make or cmake for build.<br>
    Alpha release is on hold.. The first iteration of development on this project has finished. SSL cert validation currently is broken and needs vetted.
</p>
