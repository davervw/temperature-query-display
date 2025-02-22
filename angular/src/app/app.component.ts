import { Component, OnInit } from '@angular/core';
import { HttpClient } from '@angular/common/http';

interface Temperature {
  raw : number;
  fahrenheit : number;
};

@Component({
    selector: 'app-root',
    templateUrl: './app.component.html',
    styleUrls: ['./app.component.css'],
    standalone: false
})
export class AppComponent {
  constructor(private http: HttpClient) {}
  title = 'temperature-query-display';
  url = 'http://172.27.1.24/temp';
  temp? : Temperature;
  lastUpdate : Date | '' = '';

  ngOnInit() {
    this.queryTemperature();
  }

  queryTemperature() : void {
    this.http.get<Temperature>(this.url)
      .subscribe((data: Temperature) => {
        this.temp = data;
        this.lastUpdate = new Date();
        setTimeout(() => { this.queryTemperature(); },15000);
    }); 
  }
}
