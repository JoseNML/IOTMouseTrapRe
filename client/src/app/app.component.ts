import {Component, OnInit} from '@angular/core';
import {AngularFireDatabase} from 'angularfire2/database';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit {
  isOn = false;
  isCaught = false;
  trapId = '';
  lat = null;
  lng = null;
  invalidtrap = false;

  constructor(private db: AngularFireDatabase) {

  }

  connectToTrap(event: any) {
    this.trapId = event.target.value;
    console.log('connecting to page');
    this.invalidtrap = false;
    this.getLocation();
    if (this.lat === null && this.lng === null) {
      this.invalidtrap = true;
    }
  }

  ngOnInit() {
    this.caughtSomething();
  }

  caughtSomething() {
    this.db.object(this.trapId + '/isCaught').valueChanges().subscribe(
      res => {
        if (res) {
          this.isCaught = true;
        } else {
          this.isCaught = false;
        }
      }
    );
  }

  getLocation() {
    this.db.object(this.trapId + '/latitude').valueChanges().subscribe(
      res => {
        this.lat = res;
      }
    );
    this.db.object(this.trapId + '/longtitude').valueChanges().subscribe(
      res => {
        this.lng = res;
      }
    );
  }

  changeState(trapId: string) {
    this.caughtSomething();
    this.trapId = trapId;
    console.log('Button Pressed!');
    if (this.isOn === true) {
      this.isOn = false;
    } else {
      this.isOn = true;
    }
    console.log('TrapId : ' + trapId);
    console.log('State : ' + this.isOn);
    this.db.object(trapId + '/state').set(this.isOn)
      .then(_ => {
        console.log('success');
      });
  }
}
