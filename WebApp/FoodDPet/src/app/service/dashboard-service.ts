import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, BehaviorSubject, map, catchError, of, tap } from 'rxjs';

export interface FoodRequest {
  hora: Date | string;
  horaOriginal?: string; // Guardar el formato original del ESP32
  estado: string;
}

interface FullnessResponse {
  percentage: number;
  status: string;
}

@Injectable({
  providedIn: 'root',
})
export class DashboardService {
  // IP del ESP32
  private apiUrl = 'http://192.168.100.146/api';

  // BehaviorSubjects para datos reactivos
  private percentFullnessSubject = new BehaviorSubject<number>(75);
  private fullnessStatusSubject = new BehaviorSubject<string>('Medio');
  private requestHistorySubject = new BehaviorSubject<FoodRequest[]>([]);

  // Observables públicos
  public percentFullness$ = this.percentFullnessSubject.asObservable();
  public fullnessStatus$ = this.fullnessStatusSubject.asObservable();
  public requestHistory$ = this.requestHistorySubject.asObservable();

  constructor(private http: HttpClient) {
    this.loadInitialData();
  }

  public loadInitialData(): void {
    // Cargar datos iniciales
    this.getFullnessFromAPI().subscribe();
    this.getRequestsFromAPI().subscribe();
  }

  private getFullnessFromAPI(): Observable<void> {
    return this.http.get<FullnessResponse>(`${this.apiUrl}/fullness`).pipe(
      tap((response) => {
        this.percentFullnessSubject.next(response.percentage);
        this.fullnessStatusSubject.next(response.status);
      }),
      map(() => void 0),
      catchError(() => {
        console.warn('No se pudo conectar con el ESP32, usando datos locales');
        return of(void 0);
      })
    );
  }

  private getRequestsFromAPI(): Observable<void> {
    return this.http.get<FoodRequest[]>(`${this.apiUrl}/requests`).pipe(
      tap((requests) => {
        const mappedRequests = requests.map((r) => ({
          ...r,
          horaOriginal: r.hora as string, // Guardar el formato original del ESP32
          hora: new Date(r.hora),
        }));
        this.requestHistorySubject.next(mappedRequests);
      }),
      map(() => void 0),
      catchError(() => {
        console.warn('No se pudo obtener solicitudes del ESP32');
        return of(void 0);
      })
    );
  }

  getFullnessStatus(): string {
    return this.fullnessStatusSubject.value;
  }

  getFullnessPercentage(): number {
    return this.percentFullnessSubject.value;
  }

  getRequestHistory(): FoodRequest[] {
    return this.requestHistorySubject.value;
  }

  refillDispenser(): Observable<void> {
    return this.http
      .post<{ message: string; percentage: number }>(`${this.apiUrl}/refill`, {})
      .pipe(
        tap((response) => {
          this.percentFullnessSubject.next(response.percentage);
          const status =
            response.percentage >= 75 ? 'Alto' : response.percentage >= 50 ? 'Medio' : 'Bajo';
          this.fullnessStatusSubject.next(status);
        }),
        map(() => void 0),
        catchError(() => {
          console.error('Error al rellenar el dispensador');
          return of(void 0);
        })
      );
  }

  completeRequest(request: FoodRequest): void {
    // Usar el formato original del ESP32 si está disponible
    const horaToSend = request.horaOriginal || (request.hora as string);

    this.http
      .post(`${this.apiUrl}/requests/complete`, { hora: horaToSend })
      .pipe(
        catchError((error) => {
          console.error('Error al completar solicitud', error);
          return of(null);
        })
      )
      .subscribe(() => {
        request.estado = 'Completed';
        this.getRequestsFromAPI().subscribe();
      });
  }

  cancelRequest(request: FoodRequest): void {
    // Usar el formato original del ESP32 si está disponible
    const horaToSend = request.horaOriginal || (request.hora as string);

    this.http
      .post(`${this.apiUrl}/requests/cancel`, { hora: horaToSend })
      .pipe(
        catchError((error) => {
          console.error('Error al cancelar solicitud', error);
          return of(null);
        })
      )
      .subscribe(() => {
        request.estado = 'Cancelled';
        this.getRequestsFromAPI().subscribe();
      });
  }
}
